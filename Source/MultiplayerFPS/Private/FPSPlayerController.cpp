// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerFPS/MultiplayerFPSCharacter.h"
#include "MultiplayerFPS/MultiplayerFPSGameMode.h"
#include "MultiplayerFPS/Public/FPSPlayerController.h"
#include "MultiplayerFPS/Public/FirstPersonWeapon.h"
#include "MultiplayerFPS/Public/ThirdPersonWeapon.h"
#include "Net/UnrealNetwork.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalPlayerController())
	{
		HUD = CreateWidget(this, HUDClass);
		WeaponPickupNotify = CreateWidget(this, WeaponPickupNotifyClass);
	}

	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;
		CurrentGameMode = Cast<AMultiplayerFPSGameMode>(UGameplayStatics::GetGameMode(this));
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
					OwnedPlayerCharacter->OnTakePointDamage.RemoveDynamic(this, &AFPSPlayerController::OnPointDamage);
					
					OwnedPlayerCharacter = PlayerCharacter;
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
					OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);
					OwnedPlayerCharacter->OnTakePointDamage.AddDynamic(this, &AFPSPlayerController::OnPointDamage);
				}
			}
			else
			{
				OwnedPlayerCharacter = PlayerCharacter;
				OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleBeginOverlap);
				OwnedPlayerCharacter->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFPSPlayerController::OnPlayerCapsuleEndOverlap);
				OwnedPlayerCharacter->OnTakePointDamage.AddDynamic(this, &AFPSPlayerController::OnPointDamage);
			}		
		}
	}	
}

void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Pickup", IE_Pressed, this, &AFPSPlayerController::EquipInputHandle);
}

void AFPSPlayerController::SetHUD(bool bDisplay)
{
	if (IsLocalPlayerController() && HUD)
	{
		if (bDisplay && !HUD->IsInViewport())
		{
			HUD->AddToViewport();
		}
		else if (!bDisplay && HUD->IsInViewport())
		{
			HUD->RemoveFromViewport();
		}
	}
	else
	{
		Client_SetHUD(bDisplay);
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
			if (IsLocalPlayerController() && !WeaponPickupNotify->IsInViewport())
			{
				WeaponPickupNotify->AddToViewport();
			}
			else
			{
				SetWeaponPickupNotify(true);
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("%s has Weapon tag but is not a weapon."), *OtherActor->GetName());
	}
}

void AFPSPlayerController::OnPlayerCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Weapon"))
	{
		OverlappedWeapon = nullptr;
				
		if (IsLocalPlayerController() && WeaponPickupNotify->IsInViewport())
		{
			WeaponPickupNotify->RemoveFromViewport();
		}
		else
		{
			SetWeaponPickupNotify(false);
		}
	}
}

void AFPSPlayerController::Client_SetHUD_Implementation(bool bDisplay)
{
	if (IsLocalPlayerController() && HUD)
	{
		if (bDisplay && !HUD->IsInViewport())
		{
			HUD->AddToViewport();
		}
		else if (!bDisplay && HUD->IsInViewport())
		{
			HUD->RemoveFromViewport();
		}
	}
}

void AFPSPlayerController::EquipInputHandle()
{
	Server_WeaponEquipHandler();
}

void AFPSPlayerController::Server_WeaponEquipHandler_Implementation()
{
	if (OverlappedWeapon)
	{
		AThirdPersonWeapon* Weapon = OverlappedWeapon;
		EquipWeapon(Weapon);
		
		switch (Weapon->GetWeaponType())
		{
		case EWeaponType::Primary:
			if (EquippedPrimaryWeapon)
			{
				DropWeapon(EquippedPrimaryWeapon);
			}
			EquippedPrimaryWeapon = Weapon;
			break;
		case EWeaponType::Secondary:
			if (EquippedSecondaryWeapon)
			{
				DropWeapon(EquippedSecondaryWeapon);
			}
			EquippedSecondaryWeapon = Weapon;
			break;
		case EWeaponType::Knife:
			if (EquippedKnife)
			{
				DropWeapon(EquippedKnife);
			}
			EquippedKnife = Weapon;
			break;
		}
	}
}

bool AFPSPlayerController::Server_WeaponEquipHandler_Validate()
{
	return true;
}

void AFPSPlayerController::EquipWeapon(AThirdPersonWeapon* Weapon)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);

	Weapon->EquippedWeapon(true, GetPawn());
	Weapon->AttachToComponent(OwnedPlayerCharacter->GetMesh(), AttachmentRules, Weapon->GetSocketName());
	Weapon->GetFirstPersonWeapon()->AttachToComponent(OwnedPlayerCharacter->GetMesh1P(), AttachmentRules, Weapon->GetSocketName());
		
	OwnedPlayerCharacter->SetActiveWeapon(Weapon);
}

void AFPSPlayerController::DropWeapon(AThirdPersonWeapon* Weapon)
{
	FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);

	Weapon->DetachFromActor(DetachmentRules);
	Weapon->GetFirstPersonWeapon()->DetachFromActor(DetachmentRules);
	Weapon->EquippedWeapon(false);
}

void AFPSPlayerController::SetWeaponPickupNotify_Implementation(bool bDisplay)
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

void AFPSPlayerController::OnPointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
	const UDamageType* DamageType, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	if (CurrentHealth <= 0.f)
	{
		OwnedPlayerCharacter->OnDeath();
		CurrentGameMode->RespawnHandler(this);
	}
}

void AFPSPlayerController::Respawn()
{
	CurrentHealth = MaxHealth;

	OwnedPlayerCharacter->OnRespawn();
	FTransform SpawnTransform = CurrentGameMode->GetPlayerSpawnTransform(OwnedPlayerCharacter->GetIsBlack());
	OwnedPlayerCharacter->SetActorLocationAndRotation(SpawnTransform.GetLocation(), SpawnTransform.GetRotation());
}

void AFPSPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerController, CurrentHealth);
}
