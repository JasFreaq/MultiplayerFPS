// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerFPS/MultiplayerFPS.h"
#include "MultiplayerFPS/Public/FirstPersonWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "ThirdPersonWeapon.h"
#include "MultiplayerFPS/MultiplayerFPSCharacter.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

class AMultiplayerFPSCharacter;
// Sets default values
AThirdPersonWeapon::AThirdPersonWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetCollisionProfileName("IgnoreOnlyPawn");
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponMesh->SetOwnerNoSee(true);
	
	PickupVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Pickup Volume"));
	PickupVolume->SetupAttachment(RootComponent);
	PickupVolume->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	PickupVolume->SetCollisionProfileName("OverlapOnlyPawn");
	PickupVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	/*ThirdPersonMuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThirdPersonMuzzleFlash"));
	ThirdPersonMuzzleFlash->SetAutoActivate(false);
	ThirdPersonMuzzleFlash->SetOwnerNoSee(true);

	FirstPersonMuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FirstPersonMuzzleFlash"));
	FirstPersonMuzzleFlash->SetAutoActivate(false);
	FirstPersonMuzzleFlash->SetOnlyOwnerSee(true);*/
	
	Tags.Add("Weapon");
	
	SetReplicates(true);
    AActor::SetReplicateMovement(true);

	/*ThirdPersonMuzzleFlash->SetIsReplicated(true);
	FirstPersonMuzzleFlash->SetIsReplicated(true);*/
	
	PickupVolume->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AThirdPersonWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		FirstPersonWeapon = Cast<AFirstPersonWeapon>(GetWorld()->SpawnActor(FirstPersonClass, &GetTransform(), SpawnParams));
		FirstPersonWeapon->SetOwner(this);

		/*FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
		ThirdPersonMuzzleFlash->AttachToComponent(WeaponMesh, AttachmentRules, MUZZLE_FLASH_SOCKET);
		FirstPersonMuzzleFlash->AttachToComponent(FirstPersonWeapon->GetWeaponMesh(), AttachmentRules, MUZZLE_FLASH_SOCKET);*/
	}
}

void AThirdPersonWeapon::Fire()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	GetOwner()->GetInstigatorController()->GetPlayerViewPoint(EyeLocation, EyeRotation);

	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, EyeLocation + EyeRotation.Vector() * WeaponProperties.MaxRange, SHOOTING_CHANNEL, CollisionQueryParams);

	if (HitResult.bBlockingHit)
	{
		AMultiplayerFPSCharacter* HitPlayer = Cast<AMultiplayerFPSCharacter>(HitResult.GetActor());

		if (HitPlayer)
		{
			if (!HitPlayer->ActorHasTag(OwningCharacter->GetTeamTag()))
			{
				EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
				TMap<TEnumAsByte<EPhysicalSurface>, float> Damages = HitResult.Distance > WeaponProperties.DamageFalloffRange ? WeaponProperties.FalloffDamages : WeaponProperties.MaxDamages;

				float Damage = 0;
				switch (SurfaceType)
				{
				case SURFACE_HEAD:
					if (Damages.Contains(SURFACE_HEAD))
						Damage = *Damages.Find(SURFACE_HEAD);
					break;
				case SURFACE_TORSO:
					if (Damages.Contains(SURFACE_TORSO))
						Damage = *Damages.Find(SURFACE_TORSO);
					break;
				case SURFACE_ARMS:
					if (Damages.Contains(SURFACE_ARMS))
						Damage = *Damages.Find(SURFACE_ARMS);
					break;
				case SURFACE_LEGS:
					if (Damages.Contains(SURFACE_LEGS))
						Damage = *Damages.Find(SURFACE_LEGS);
					break;
				default:
					Damage = 0;
					UE_LOG(LogTemp, Error, TEXT("Player(%s) hit has missing SurfaceType in Physics Asset."), *HitPlayer->GetName());
					break;
				}

				UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, EyeRotation.Vector(), HitResult, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
			}
		}
		else if (WeaponProperties.BulletHoleDecal)
		{
			SpawnDecal(HitResult);
		}
	}

	OnFireEffects(/*true*/);

	//// try and play the sound if specified
	//if (FireSound != NULL)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}
}

void AThirdPersonWeapon::SpawnDecal_Implementation(FHitResult HitResult)
{
	FVector Scale(8.f, 8.f, 8.f);
	FRotator Rotation = FRotator(HitResult.Normal.Rotation().Pitch + 180, HitResult.Normal.Rotation().Yaw, HitResult.Normal.Rotation().Roll + 180);
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(this, WeaponProperties.BulletHoleDecal, Scale, HitResult.Location, Rotation, 8.f);
	if (Decal)
	{
		Decal->SetFadeScreenSize(0.0001f);
	}
}

void AThirdPersonWeapon::EquippedWeapon(bool bEquipped, AActor* NewOwner)
{
	if (bEquipped)
	{
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FirstPersonWeapon->GetWeaponMesh()->SetVisibility(true);
		PickupVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetOwner(NewOwner);
		FirstPersonWeapon->SetOwner(NewOwner);
		OwningCharacter = Cast<AMultiplayerFPSCharacter>(NewOwner);
	}
	else
	{
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		FirstPersonWeapon->GetWeaponMesh()->SetVisibility(false);
		PickupVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		SetOwner(nullptr);
		FirstPersonWeapon->SetOwner(this);
		OwningCharacter = nullptr;
	}
}

void AThirdPersonWeapon::StartFire()
{
	Fire();
}

void AThirdPersonWeapon::StopFire()
{
	
}

void AThirdPersonWeapon::OnFireEffects_Implementation(/*bool bActivate*/)
{
	/*
	This code and other places where these were used were commented out because the animations
	already have effects and sound and it was cleaner to use them. However, I have commented
	the code instead of deleting it as it may prove useful for reference as an alternate way to
	set up effects if they aren't set up in the animations.
	*/
	//ThirdPersonMuzzleFlash->Activate(bActivate);
	//FirstPersonMuzzleFlash->Activate(bActivate);

	if (WeaponProperties.FireAnim)
	{
		if (GetOwner()->GetInstigatorController())
		{
			if (GetOwner()->GetInstigatorController()->IsLocalPlayerController())
			{
				FirstPersonWeapon->GetWeaponMesh()->PlayAnimation(WeaponProperties.FireAnim, false);
			}
			else
			{
				WeaponMesh->PlayAnimation(WeaponProperties.FireAnim, false);
			}
		}
		else
		{
			WeaponMesh->PlayAnimation(WeaponProperties.FireAnim, false);
		}
	}
}

void AThirdPersonWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThirdPersonWeapon, FirstPersonWeapon);
	DOREPLIFETIME(AThirdPersonWeapon, OwningCharacter);
}
