// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonWeapon.h"
#include "ThirdPersonWeapon_Automatic.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AThirdPersonWeapon_Automatic : public AThirdPersonWeapon
{
	GENERATED_BODY()

public:	
	virtual void StartFire() override;

	virtual void StopFire() override;

protected:
	virtual void Fire() override;
	
private:
	UFUNCTION(Client, Reliable)
		void RecoilHandler();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ToolTip = "No. of bullets shot per second. A value of 1 means that the weapon is not automatic.", ClampMin = "2", ClampMax = "8", UIMin = "2", UIMax = "8", AllowPrivateAccess = true))
		int32 RateOfFire = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		bool bIsSemiAutomatic = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ToolTip = "No. of bullets shot per burst. Only valid for SemiAutomatics.", AllowPrivateAccess = true))
		float BurstCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		UCurveFloat* VerticalRecoilBeginCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		UCurveFloat* VerticalRecoilRepeatCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		UCurveFloat* HorizontalRecoilCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ToolTip = "No. of bullets to be shot before horizontal recoil begins.", AllowPrivateAccess = true))
		int32 HorizontalRecoilRequirement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ClampMin = "0.1", ClampMax = "0.6", UIMin = "0.1", UIMax = "0.6", AllowPrivateAccess = true))
		float VerticalDeviationLimit = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (ClampMin = "0.1", ClampMax = "0.6", UIMin = "0.1", UIMax = "0.6", AllowPrivateAccess = true))
		float HorizontalDeviationLimit = 0.1f;

	UPROPERTY(Replicated)
		int32 BulletsSpent = 0;

	float OldVerticalRecoil;

	float OldHorizontalRecoil;
	
	FTimerHandle FireTimerHandle;
	
	FTimerHandle BurstTimerHandle;

	float LastFireTime;
};
