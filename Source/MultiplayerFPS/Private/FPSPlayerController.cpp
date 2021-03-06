// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "MultiplayerFPS/MultiplayerFPSCharacter.h"
#include "MultiplayerFPS/Public/FPSPlayerController.h"
#include "MultiplayerFPS/Public/FirstPersonWeapon.h"
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
					/*OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.RemoveDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);*/
					
					OwnedPlayerCharacter = PlayerCharacter;
					/*OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);*/
				}
			}
			else
			{
				OwnedPlayerCharacter = PlayerCharacter;
				/*OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
				OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);*/
			}		
		}
	}	
}

void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Pickup", IE_Pressed, this, &AFPSPlayerController::EquipInputHandle);
}

//void AFPSPlayerController::OnPlayerCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
//                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor->ActorHasTag("Weapon"))
//	{
//		AThirdPersonWeapon* Weapon = Cast<AThirdPersonWeapon>(OtherActor);
//		if (Weapon)
//		{
//			OverlappedWeapon = Weapon;
//			if (IsLocalPlayerController() && !WeaponPickupNotify->IsInViewport())
//			{
//				WeaponPickupNotify->AddToViewport();
//			}
//			else
//			{
//				DisplayWeaponPickupNotify(true);
//			}
//		}
//		else
//			UE_LOG(LogTemp, Warning, TEXT("%s has Weapon tag but is not a weapon."), *OtherActor->GetName());
//	}
//}
//
//void AFPSPlayerController::OnPlayerCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent,
//	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	if (OtherActor->ActorHasTag("Weapon"))
//	{
//		AThirdPersonWeapon* Weapon = nullptr;
//		switch (OverlappedWeapon->GetWeaponType())
//		{
//		case EWeaponType::Primary:
//			if (EquippedPrimaryWeapon)
//			{
//				Weapon = EquippedPrimaryWeapon;
//			}
//			break;
//		case EWeaponType::Secondary:
//			if (EquippedSecondaryWeapon)
//			{
//				Weapon = EquippedSecondaryWeapon;
//			}
//			break;
//		case EWeaponType::Knife:
//			if (EquippedKnife)
//			{
//				Weapon = EquippedKnife;
//			}
//			break;
//		}
//
//		if (Weapon != OverlappedWeapon)
//			OverlappedComponent = nullptr;
//		
//		if (IsLocalPlayerController() && WeaponPickupNotify->IsInViewport())
//		{
//			WeaponPickupNotify->RemoveFromViewport();
//		}
//		else
//		{
//			DisplayWeaponPickupNotify(false);
//		}
//	}
//}

void AFPSPlayerController::OnPlayerBeginOverlapWeapon(AThirdPersonWeapon* OverlappedWeapon)
{
	
}

void AFPSPlayerController::OnPlayerEndOverlapWeapon(AThirdPersonWeapon* OverlappedWeapon)
{
	
}

void AFPSPlayerController::EquipInputHandle()
{
	Server_WeaponEquipHandler();
}

void AFPSPlayerController::Server_WeaponEquipHandler_Implementation()
{
	if (OverlappedWeapon)
	{
		switch (OverlappedWeapon->GetWeaponType())
		{
		case EWeaponType::Primary:
			if (EquippedPrimaryWeapon)
			{
				DropWeapon(EquippedPrimaryWeapon);
			}
			EquippedPrimaryWeapon = OverlappedWeapon;
			break;
		case EWeaponType::Secondary:
			if (EquippedSecondaryWeapon)
			{
				DropWeapon(EquippedSecondaryWeapon);
			}
			EquippedSecondaryWeapon = OverlappedWeapon;
			break;
		case EWeaponType::Knife:
			if (EquippedKnife)
			{
				DropWeapon(EquippedKnife);
			}
			EquippedKnife = OverlappedWeapon;
			break;
		}

		EquipWeapon(OverlappedWeapon);
	}
}

bool AFPSPlayerController::Server_WeaponEquipHandler_Validate()
{
	return true;
}

void AFPSPlayerController::EquipWeapon(AThirdPersonWeapon* Weapon)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(GetLocalRole()));
	Weapon->EquippedWeapon(true, this);
	Weapon->AttachToComponent(OwnedPlayerCharacter->GetMesh(), AttachmentRules, Weapon->GetSocketName());
	Weapon->GetFirstPersonWeapon()->AttachToComponent(OwnedPlayerCharacter->GetMesh1P(), AttachmentRules, Weapon->GetSocketName());
}

void AFPSPlayerController::DropWeapon(AThirdPersonWeapon* Weapon)
{
	FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);

	Weapon->EquippedWeapon(false, this);
	Weapon->DetachFromActor(DetachmentRules);
	Weapon->GetFirstPersonWeapon()->DetachFromActor(DetachmentRules);
}

void AFPSPlayerController::DisplayWeaponPickupNotify_Implementation(bool bDisplay)
{
	if (IsLocalPlayerController() && WeaponPickupNotify)
	{
		if (bDisplay && !WeaponPickupNotify->IsInViewport())
		{
			WeaponPickupNotify->AddToViewport();
		}
		else if (!bDisplay && WeaponPickupNotify->IsInViewport())
		{
			WeaponPickupNotify->RemoveFromViewport();
		}
	}
}
