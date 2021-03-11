// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerFPS/MultiplayerFPSCharacter.h"
#include "ThirdPersonWeapon_Automatic.h"

#include "Net/UnrealNetwork.h"

#define BEGIN_RECOIL_CURVE_LIMIT 10

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
	if (GetOwner()->GetInstigatorController()->IsLocalPlayerController())
	{
		//Calculate Base Recoil Amounts
		float VerticalRecoilAmount;
		float HorizontalRecoilAmount;

		float VerticalRecoil;
		float HorizontalRecoil = 0;

		if (BulletsSpent <= 1)
		{
			OldVerticalRecoil = 0;
			OldHorizontalRecoil = 0;
		}		
		
		if (BulletsSpent <= BEGIN_RECOIL_CURVE_LIMIT)
		{
			VerticalRecoil = VerticalRecoilBeginCurve->GetFloatValue(BulletsSpent);
		}
		else
		{
			VerticalRecoil = VerticalRecoilRepeatCurve->GetFloatValue(BulletsSpent - BEGIN_RECOIL_CURVE_LIMIT);
		}

		if (BulletsSpent > HorizontalRecoilRequirement)
		{
			HorizontalRecoil = HorizontalRecoilCurve->GetFloatValue(BulletsSpent - HorizontalRecoilRequirement);
		}		
		
		//Adjust Recoil Amounts
		VerticalRecoilAmount = VerticalRecoil - OldVerticalRecoil;
		HorizontalRecoilAmount = HorizontalRecoil - OldHorizontalRecoil;

		if (BulletsSpent != 1 && OwningCharacter->GetIsRunning())
		{
			float VerticalDeviation = FMath::RandRange(-VerticalDeviationLimit, VerticalDeviationLimit);
			float HorizontalDeviation = FMath::RandRange(-HorizontalDeviationLimit, HorizontalDeviationLimit);

			VerticalRecoilAmount += VerticalDeviation;
			HorizontalRecoilAmount += HorizontalDeviation;
		}
		
		if ((BulletsSpent - HorizontalRecoilRequirement) % BEGIN_RECOIL_CURVE_LIMIT > 0)
		{
			float HorizontalDeviationMultiplier = 1 + ((BulletsSpent - HorizontalRecoilRequirement) % BEGIN_RECOIL_CURVE_LIMIT) * 0.2;

			HorizontalRecoilAmount *= HorizontalDeviationMultiplier;
		}
				
		FRotator ControlRotation = GetOwner()->GetInstigatorController()->GetControlRotation();
		GetOwner()->GetInstigatorController()->SetControlRotation(FRotator(ControlRotation.Pitch + VerticalRecoilAmount, ControlRotation.Yaw + HorizontalRecoilAmount, ControlRotation.Roll));

		OldVerticalRecoil = VerticalRecoil;
		OldHorizontalRecoil = HorizontalRecoil;
	}
}

void AThirdPersonWeapon_Automatic::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThirdPersonWeapon_Automatic, BulletsSpent);
}