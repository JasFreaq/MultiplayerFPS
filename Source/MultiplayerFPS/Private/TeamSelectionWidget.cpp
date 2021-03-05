// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Button.h"
#include "MultiplayerFPS/Public/AMapPlayerControllerInterface.h"
#include "TeamSelectionWidget.h"

bool UTeamSelectionWidget::Initialize()
{
	if (Super::Initialize())
	{
		PlayerController = Cast<IAMapPlayerControllerInterface>(GetOwningPlayer());
		if (PlayerController == nullptr)
			return false;

		if (BlackButton)
			BlackButton->OnClicked.AddDynamic(this, &UTeamSelectionWidget::SelectBlack);
		else
			return false;

		if (SilverButton)
			SilverButton->OnClicked.AddDynamic(this, &UTeamSelectionWidget::SelectSilver);
		else
			return false;

		return true;
	}
	
	return false;
}

void UTeamSelectionWidget::SelectBlack()
{
	if (ensure(PlayerController != nullptr))
	{
		PlayerController->UpdateTeamSelection(true);
	}
}

void UTeamSelectionWidget::SelectSilver()
{
	if (ensure(PlayerController != nullptr))
	{
		PlayerController->UpdateTeamSelection(false);
	}
}
