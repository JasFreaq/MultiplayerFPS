// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirstPersonWeapon.generated.h"

UCLASS()
class MULTIPLAYERFPS_API AFirstPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFirstPersonWeapon();

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return  WeaponMesh; }
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
		USkeletalMeshComponent* WeaponMesh = nullptr;
};
