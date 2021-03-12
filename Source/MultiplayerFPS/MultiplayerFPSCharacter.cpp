// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerFPSCharacter.h"
#include "MultiplayerFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "MultiplayerFPS.h"
#include "MultiplayerFPS/Public/ThirdPersonWeapon.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

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

void AMultiplayerFPSCharacter::SetActiveWeapon_Implementation(AThirdPersonWeapon* NewWeapon)
{
	if (HasAuthority())
		ActiveWeapon = NewWeapon;

	if (NewWeapon->GetWeaponType() == EWeaponType::Knife)
		bIsCurrentlyUsingKnife = true;
	else
		bIsCurrentlyUsingKnife = false;
}

void AMultiplayerFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (HasAuthority())
		InitialMeshRelativeLocation = GetMesh()->GetRelativeLocation();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMultiplayerFPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMultiplayerFPSCharacter::StopFire);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMultiplayerFPSCharacter::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMultiplayerFPSCharacter::StopRunning);

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

void AMultiplayerFPSCharacter::StartFire()
{
	if (!HasAuthority())
	{
		Server_StartFire();
		return;
	}
		
	if (!bIsDead && ActiveWeapon)
	{
		bIsShooting = true;
		ActiveWeapon->StartFire();
	}
}

void AMultiplayerFPSCharacter::StopFire()
{
	if (!HasAuthority())
	{
		Server_StopFire();
		return;
	}
		
	if (ActiveWeapon)
	{
		bIsShooting = false;
		ActiveWeapon->StopFire();
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

void AMultiplayerFPSCharacter::Run()
{
	if (!bIsDead)
	{
		if (HasAuthority())
			Multi_Run(true);
		else
			Server_Run(true);
	}
}

void AMultiplayerFPSCharacter::StopRunning()
{
	if (HasAuthority())
		Multi_Run(false);
	else
		Server_Run(false);
}

void AMultiplayerFPSCharacter::Server_Run_Implementation(bool bRun)
{
	Multi_Run(bRun);
}

bool AMultiplayerFPSCharacter::Server_Run_Validate(bool bRun)
{
	return true;
}

void AMultiplayerFPSCharacter::Multi_Run_Implementation(bool bRun)
{
	bIsRunning = bRun;
	GetCharacterMovement()->MaxWalkSpeed = bRun ? RunSpeed : WalkSpeed;
}

void AMultiplayerFPSCharacter::Server_StartFire_Implementation()
{
	StartFire();
}

bool AMultiplayerFPSCharacter::Server_StartFire_Validate()
{
	return true;
}

void AMultiplayerFPSCharacter::Server_StopFire_Implementation()
{
	StopFire();
}

bool AMultiplayerFPSCharacter::Server_StopFire_Validate()
{
	return true;
}

void AMultiplayerFPSCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerFPSCharacter, bIsShooting);
}
