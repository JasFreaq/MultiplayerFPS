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
class MULTIPLAYERFPS_API AFPSPlayerController : public APlayerController, public IPlayerControllerInterface
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = UI)
		TSubclassOf<UUserWidget> WeaponPickupNotifyClass;
	
	class AMultiplayerFPSCharacter* OwnedPlayerCharacter = nullptr;

	AThirdPersonWeapon* EquippedPrimaryWeapon = nullptr;
	
	AThirdPersonWeapon* EquippedSecondaryWeapon = nullptr;
	
	AThirdPersonWeapon* EquippedKnife = nullptr;

	AThirdPersonWeapon* OverlappedWeapon = nullptr;

	UPROPERTY()
		UUserWidget* WeaponPickupNotify = nullptr;
	
	
	virtual void OnPlayerBeginOverlapWeapon(AThirdPersonWeapon* Weapon) override;

	virtual void OnPlayerEndOverlapWeapon(AThirdPersonWeapon* Weapon) override;

	/*UFUNCTION()
		void OnPlayerCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnPlayerCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

	UFUNCTION(Client, Reliable)
		void DisplayWeaponPickupNotify(bool bDisplay);

	void EquipInputHandle();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_WeaponEquipHandler();

	void EquipWeapon(AThirdPersonWeapon* Weapon);

	void DropWeapon(AThirdPersonWeapon* Weapon);
};
