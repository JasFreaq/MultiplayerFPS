// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerFPS/MultiplayerFPSGameMode.h"
#include "AMapFPSGameMode.generated.h"

class AMultiplayerFPSCharacter;

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AAMapFPSGameMode : public AMultiplayerFPSGameMode
{
	GENERATED_BODY()

public:
	AAMapFPSGameMode();

	void SpawnPlayerCharacter(APlayerController* NewPlayer, bool bBlack);

private:
	void SetViewToLoginCamera(APlayerController* NewPlayer);
	
	void SetViewToLoginCamera_Delayed(APlayerController* NewPlayer);
	
	UPROPERTY(EditDefaultsOnly, Category = Teams)
		TSubclassOf<AMultiplayerFPSCharacter> BlackCharacter;

	UPROPERTY(EditDefaultsOnly, Category = Teams)
		TSubclassOf<AMultiplayerFPSCharacter> SilverCharacter;
	
	AActor* LoginCam = nullptr;

	TQueue<FTimerHandle> LoginTimerHandles;

	TArray<AActor*> BlackSpawnPoints;
	
	TArray<AActor*> SilverSpawnPoints;
			
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;	
};
