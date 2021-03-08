// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFirstPersonWeapon::AFirstPersonWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionProfileName("NoCollision");
	WeaponMesh->SetOnlyOwnerSee(true);
	WeaponMesh->SetVisibility(false);
	
	SetReplicates(true);
	AActor::SetReplicateMovement(true);
}