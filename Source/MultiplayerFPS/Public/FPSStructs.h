#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "FPSStructs.generated.h"

USTRUCT(BlueprintType)
struct FWeaponProperties
{
    GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<TEnumAsByte<EPhysicalSurface>, float> MaxDamages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<TEnumAsByte<EPhysicalSurface>, float> FalloffDamages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageFalloffRange;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimBlueprint* ThirdPersonAnimOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimBlueprint* FirstPersonAnimOverride;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundBase* FireSound;
};