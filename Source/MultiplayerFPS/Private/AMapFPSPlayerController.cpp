// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerFPS/Public/AMapFPSGameMode.h"
#include "AMapFPSPlayerController.h"

void AAMapFPSPlayerController::DisplayTeamSelectionWidget()
{
	if (IsLocalPlayerController())
	{
		TeamSelectorWidget = CreateWidget(this, TeamSelectorClass.Get());
		TeamSelectorWidget->AddToViewport();
	}
	else
	{
		Client_DisplayTeamSelectionWidget();
	}
}

void AAMapFPSPlayerController::UpdateTeamSelection(bool bBlack)
{
	if (HasAuthority())
	{
		GameMode->SpawnPlayerCharacter(this, bBlack);
	}
	else
	{
		Server_UpdateTeamSelection(bBlack);
	}

	TeamSelectorWidget->RemoveFromParent();
	TeamSelectorWidget = nullptr;
}

void AAMapFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AAMapFPSGameMode>(UGameplayStatics::GetGameMode(this));
}

void AAMapFPSPlayerController::Server_UpdateTeamSelection_Implementation(bool bBlack)
{
	GameMode->SpawnPlayerCharacter(this, bBlack);
}

bool AAMapFPSPlayerController::Server_UpdateTeamSelection_Validate(bool bBlack)
{
	return true;
}

void AAMapFPSPlayerController::Client_DisplayTeamSelectionWidget_Implementation()
{
	TeamSelectorWidget = CreateWidget(this, TeamSelectorClass.Get());
	TeamSelectorWidget->AddToViewport();
}
