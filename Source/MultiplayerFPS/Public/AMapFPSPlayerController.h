// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSPlayerController.h"
#include "MultiplayerFPS/Public/AMapPlayerControllerInterface.h"
#include "AMapFPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AAMapFPSPlayerController : public AFPSPlayerController, public IAMapPlayerControllerInterface
{
	GENERATED_BODY()
	
public:
	void DisplayTeamSelectionWidget();

	virtual void UpdateTeamSelection(bool bBlack) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(Client, Reliable)
		void Client_DisplayTeamSelectionWidget();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_UpdateTeamSelection(bool bBlack);

	UPROPERTY(EditDefaultsOnly, Category = UI)
		TSubclassOf<UUserWidget> TeamSelectorClass;

	UUserWidget* TeamSelectorWidget = nullptr;

	class AAMapFPSGameMode* GameMode = nullptr;
};
