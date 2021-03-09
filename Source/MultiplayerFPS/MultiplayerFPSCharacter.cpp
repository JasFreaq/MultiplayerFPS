// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerFPSCharacter.h"
#include "MultiplayerFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "MultiplayerFPS.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMultiplayerFPSCharacter

AMultiplayerFPSCharacter::AMultiplayerFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(32.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(SHOOTING_CHANNEL, ECR_Ignore);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
	Tags.Add("Player");
}

void AMultiplayerFPSCharacter::OnDeath_Implementation()
{
	bIsDead = true;

	if (bWasJumping && GetMovementComponent()->IsFalling())
		StopJumping();

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
}

void AMultiplayerFPSCharacter::OnRespawn_Implementation()
{
	bIsDead = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	
	GetMesh()->AttachTo(RootComponent, NAME_None, EAttachLocation::SnapToTarget, false);
	GetMesh()->SetRelativeLocation(InitialMeshRelativeLocation);
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}

void AMultiplayerFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (HasAuthority())
		InitialMeshRelativeLocation = GetMesh()->GetRelativeLocation();
	
	Tags.Add(TeamTag);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMultiplayerFPSCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMultiplayerFPSCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiplayerFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiplayerFPSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiplayerFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiplayerFPSCharacter::LookUpAtRate);
}

void AMultiplayerFPSCharacter::OnFire()
{
	if (!bIsDead)
	{
		if (!HasAuthority())
		{
			Server_Fire();
			return;
		}

		if (ActiveWeapon.OwningWeapon)
		{
			FVector EyeLocation;
			FRotator EyeRotation;
			GetActorEyesViewPoint(EyeLocation, EyeRotation);

			FHitResult HitResult;
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(this);
			CollisionQueryParams.AddIgnoredActor(ActiveWeapon.OwningWeapon);
			CollisionQueryParams.bTraceComplex = true;
			CollisionQueryParams.bReturnPhysicalMaterial = true;

			GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, EyeLocation + EyeRotation.Vector() * ActiveWeapon.MaxRange, SHOOTING_CHANNEL, CollisionQueryParams);
			DrawDebugLine(GetWorld(), EyeLocation, HitResult.ImpactPoint, FColor::Red, true);

			if (HitResult.bBlockingHit)
			{
				AMultiplayerFPSCharacter* HitPlayer = Cast<AMultiplayerFPSCharacter>(HitResult.GetActor());

				if (HitPlayer)
				{
					if (!HitPlayer->ActorHasTag(TeamTag))
					{
						EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
						TMap<TEnumAsByte<EPhysicalSurface>, float> Damages = HitResult.Distance > ActiveWeapon.DamageFalloffRange ? ActiveWeapon.FalloffDamages : ActiveWeapon.MaxDamages;

						float Damage;
						switch (SurfaceType)
						{
						case SURFACE_HEAD:
							Damage = *Damages.Find(SURFACE_HEAD);
							break;
						case SURFACE_TORSO:
							Damage = *Damages.Find(SURFACE_TORSO);
							break;
						case SURFACE_ARMS:
							Damage = *Damages.Find(SURFACE_ARMS);
							break;
						case SURFACE_LEGS:
							Damage = *Damages.Find(SURFACE_LEGS);
							break;
						default:
							Damage = 0;
							UE_LOG(LogTemp, Error, TEXT("Player(%s) hit has missing SurfaceType in Physics Asset."), *HitPlayer->GetName());
							break;
						}

						UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, EyeRotation.Vector(), HitResult, GetController(), ActiveWeapon.OwningWeapon, UDamageType::StaticClass());
					}
				}
				else
				{

				}
			}
		}

		//// try and play the sound if specified
		//if (FireSound != NULL)
		//{
		//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		//}

		//// try and play a firing animation if specified
		//if (FireAnimation != NULL)
		//{
		//	// Get the animation object for the arms mesh
		//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		//	if (AnimInstance != NULL)
		//	{
		//		AnimInstance->Montage_Play(FireAnimation, 1.f);
		//	}
		//}
	}
}

void AMultiplayerFPSCharacter::Jump()
{
	if (!bIsDead)
		Super::Jump();
}

void AMultiplayerFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f && !bIsDead)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMultiplayerFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f && !bIsDead)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMultiplayerFPSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerFPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerFPSCharacter::Server_Fire_Implementation()
{
	OnFire();
}

bool AMultiplayerFPSCharacter::Server_Fire_Validate()
{
	return true;
}