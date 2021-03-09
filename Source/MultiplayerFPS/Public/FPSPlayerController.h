// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerFPS/Public/PlayerControllerInterface.h"
#include "FPSPlayerController.generated.h"

class AThirdPersonWeapon;

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AFPSPlayerController : public APlayerController/*, public IPlayerControllerInterface*/
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void SetupInputComponent() override;

public:
	void SetHUD(bool bDisplay);

	void Respawn();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = UI)
		TSubclassOf<UUserWidget> HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = UI)
		TSubclassOf<UUserWidget> WeaponPickupNotifyClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerStats, meta = (AllowPrivateAccess = "true"))
		float MaxHealth;

	UPROPERTY(Replicated)
		float CurrentHealth;
	
	class AMultiplayerFPSCharacter* OwnedPlayerCharacter = nullptr;

	class AMultiplayerFPSGameMode* CurrentGameMode = nullptr;
	
	AThirdPersonWeapon* EquippedPrimaryWeapon = nullptr;
	
	AThirdPersonWeapon* EquippedSecondaryWeapon = nullptr;
	
	AThirdPersonWeapon* EquippedKnife = nullptr;

	AThirdPersonWeapon* OverlappedWeapon = nullptr;

	UPROPERTY()
		UUserWidget* HUD = nullptr;

	UPROPERTY()
		UUserWidget* WeaponPickupNotify = nullptr;

	UFUNCTION()
		void OnPlayerCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnPlayerCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Client, Reliable)
		void Client_SetHUD(bool bDisplay);
	
	UFUNCTION(Client, Reliable)
		void SetWeaponPickupNotify(bool bDisplay);

	void EquipInputHandle();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_WeaponEquipHandler();

	void EquipWeapon(AThirdPersonWeapon* Weapon);

	void DropWeapon(AThirdPersonWeapon* Weapon);

	UFUNCTION()
		void OnPointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
};
