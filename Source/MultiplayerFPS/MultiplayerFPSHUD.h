// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiplayerFPSHUD.generated.h"

UCLASS()
class AMultiplayerFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMultiplayerFPSHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

