// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerFPSGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual FTransform GetPlayerSpawnTransform(bool bBlack);

	virtual void RespawnHandler(class AFPSPlayerController* RespawnController);
};



