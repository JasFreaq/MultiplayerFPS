// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerFPS/Public/FPSStructs.h"
#include "MultiplayerFPSCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AMultiplayerFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Team, meta = (AllowPrivateAccess = "true"))
		bool bIsBlack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Team, meta = (AllowPrivateAccess = "true"))
		FName TeamTag;

	FWeaponProperties ActiveWeapon;

	FVector InitialMeshRelativeLocation;
	
	bool bIsDead = false;
	
public:
	AMultiplayerFPSCharacter();

	UFUNCTION(NetMulticast, Reliable)
		void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
		void OnRespawn();
	
protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
		
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AMultiplayerFPSProjectile> ProjectileClass;
	
protected:	
	/** Fires a projectile. */
	void OnFire();

	virtual void Jump() override;
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Fire();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
	
public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	FORCEINLINE bool GetIsBlack() const { return bIsBlack; }
	
	FORCEINLINE FName GetTeamTag() const { return TeamTag; }

	FORCEINLINE void SetActiveWeapon(FWeaponProperties NewWeapon) { ActiveWeapon = NewWeapon; }

	FORCEINLINE bool GetIsDead() const { return bIsDead; }
};

