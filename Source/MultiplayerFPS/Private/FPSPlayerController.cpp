// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "MultiplayerFPS/MultiplayerFPSCharacter.h"
#include "MultiplayerFPS/Public/FPSPlayerController.h"
#include "MultiplayerFPS/Public/ThirdPersonWeapon.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		WeaponPickupNotify = CreateWidget(this, WeaponPickupNotifyClass);
	}
}

void AFPSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (HasAuthority())
	{
		AMultiplayerFPSCharacter* PlayerCharacter = Cast<AMultiplayerFPSCharacter>(InPawn);
		if (PlayerCharacter)
		{
			if (OwnedPlayerCharacter)
			{
				if (OwnedPlayerCharacter != PlayerCharacter)
				{
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.RemoveDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);
					
					OwnedPlayerCharacter = PlayerCharacter;
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);
				}
			}
			else
			{
				OwnedPlayerCharacter = PlayerCharacter;
				OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
				OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);
			}		
		}
	}	
}

void AFPSPlayerController::OnPlayerCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Weapon"))
	{
		AThirdPersonWeapon* Weapon = Cast<AThirdPersonWeapon>(OtherActor);
		if (Weapon)
		{
			OverlappedWeapon = Weapon;
			DisplayWeaponPickupNotify(true);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("%s has Weapon tag but is not a weapon."), *OtherActor->GetName());
	}
}

void AFPSPlayerController::OnPlayerCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedWeapon = nullptr;
	DisplayWeaponPickupNotify(false);
}

void AFPSPlayerController::WeaponEquipHandler()
{
	switch (OverlappedWeapon->GetWeaponType())
	{
	case EWeaponType::Primary:
		if (EquippedPrimaryWeapon)
		{
			DropWeapon(EquippedPrimaryWeapon);
		}
		
		break;
	case EWeaponType::Secondary:
		if (EquippedSecondaryWeapon)
		{
			DropWeapon(EquippedSecondaryWeapon);
		}
		
		break;
	case EWeaponType::Knife:
		if (EquippedKnife)
		{
			DropWeapon(EquippedKnife);
		}
		
		break;
	}
}

void AFPSPlayerController::EquipWeapon(AThirdPersonWeapon* Weapon)
{

}

void AFPSPlayerController::DropWeapon(AThirdPersonWeapon* Weapon)
{
	
}

void AFPSPlayerController::DisplayWeaponPickupNotify_Implementation(bool bDisplay)
{
	if (IsLocalPlayerController())
	{
		if (bDisplay)
		{
			WeaponPickupNotify->AddToViewport();
		}
		else
		{
			WeaponPickupNotify->RemoveFromViewport();
		}
	}
}
