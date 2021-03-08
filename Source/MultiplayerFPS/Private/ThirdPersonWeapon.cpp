// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerFPS/Public/FirstPersonWeapon.h"
#include "MultiplayerFPS/Public/PlayerControllerInterface.h"
#include "ThirdPersonWeapon.h"

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

	Tags.Add("Weapon");
	
	SetReplicates(true);
    AActor::SetReplicateMovement(true);

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
	}

	//PickupVolume->OnComponentBeginOverlap.AddDynamic(this, &AThirdPersonWeapon::OnBeginOverlap);
	//PickupVolume->OnComponentEndOverlap.AddDynamic(this, &AThirdPersonWeapon::OnEndOverlap);
}

void AThirdPersonWeapon::EquippedWeapon(bool bEquipped)
{
	if (bEquipped)
	{
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FirstPersonWeapon->GetWeaponMesh()->SetVisibility(true);
		PickupVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		FirstPersonWeapon->GetWeaponMesh()->SetVisibility(false);
		PickupVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AThirdPersonWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThirdPersonWeapon, FirstPersonWeapon);
}