// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiplayerFPS/Public/FPSStructs.h"
#include "ThirdPersonWeapon.generated.h"

class AFirstPersonWeapon;
class AMultiplayerFPSCharacter;

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

	virtual void Fire();
	
	UFUNCTION(NetMulticast, Reliable)
		virtual void OnFireEffects(bool bActivate);
	
public:
	virtual void EquippedWeapon(bool bEquipped, AActor* NewOwner = nullptr);

	virtual void StartFire();

	virtual void StopFire();
		
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = true))
		UParticleSystemComponent* ThirdPersonMuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = true))
		UParticleSystemComponent* FirstPersonMuzzleFlash;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		TSubclassOf<AFirstPersonWeapon> FirstPersonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		FName SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
		FWeaponProperties WeaponProperties;
		
	UPROPERTY(Replicated)
		AFirstPersonWeapon* FirstPersonWeapon = nullptr;

	AMultiplayerFPSCharacter* OwningCharacter = nullptr;
};