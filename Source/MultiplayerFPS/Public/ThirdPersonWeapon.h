// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiplayerFPS/Public/FPSStructs.h"
#include "ThirdPersonWeapon.generated.h"

class AFirstPersonWeapon;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Primary,
	Secondary,
	Knife
};

UCLASS()
class MULTIPLAYERFPS_API AThirdPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThirdPersonWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void EquippedWeapon(bool bEquipped);
	
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	FORCEINLINE AFirstPersonWeapon* GetFirstPersonWeapon() const { return FirstPersonWeapon; }
	
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE FName GetSocketName() const { return SocketName; }

	FORCEINLINE FWeaponProperties GetWeaponProperties() const { return WeaponProperties; }
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
		USkeletalMeshComponent* WeaponMesh = nullptr;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
		class USphereComponent* PickupVolume = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		TSubclassOf<AFirstPersonWeapon> FirstPersonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		FName SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		FWeaponProperties WeaponProperties;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		UAnimBlueprint* ThirdPersonAnimOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		UAnimBlueprint* FirstPersonAnimOverride;*/

	UPROPERTY(Replicated)
		AFirstPersonWeapon* FirstPersonWeapon = nullptr;
};