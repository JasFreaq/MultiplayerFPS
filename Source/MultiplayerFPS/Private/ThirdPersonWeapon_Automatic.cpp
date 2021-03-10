// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerFPS/MultiplayerFPSCharacter.h"
#include "ThirdPersonWeapon_Automatic.h"

void AThirdPersonWeapon_Automatic::EquippedWeapon(bool bEquipped, AActor* NewOwner)
{
	Super::EquippedWeapon(bEquipped, NewOwner);

	if (bEquipped)
	{
		BaseControlRotation = NewOwner->GetInstigatorController()->GetControlRotation();
	}
	else
	{
		BaseControlRotation = FRotator::ZeroRotator;
	}
}

void AThirdPersonWeapon_Automatic::StartFire()
{
	float Rate = 1.f / RateOfFire;
	float FirstDelay = FMath::Max(0.f, LastFireTime + Rate - GetWorld()->TimeSeconds);

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AThirdPersonWeapon_Automatic::Fire, Rate, true, FirstDelay);
	
	if (bIsSemiAutomatic)
	{
		GetWorldTimerManager().SetTimer(BurstTimerHandle, this, &AThirdPersonWeapon_Automatic::StopFire, BurstCount * Rate);
	}
}

void AThirdPersonWeapon_Automatic::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	if (bIsSemiAutomatic && BurstTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
	}

	BulletsSpent = 0;
}

void AThirdPersonWeapon_Automatic::Fire()
{
	Super::Fire();

	BulletsSpent++;
	LastFireTime = GetWorld()->TimeSeconds;

	RecoilHandler();
}

void AThirdPersonWeapon_Automatic::RecoilHandler_Implementation()
{
	if (OwningCharacter->GetInstigatorController()->IsLocalPlayerController())
	{
		
	}
}
