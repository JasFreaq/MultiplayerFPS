// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API UTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
private:
	UPROPERTY(meta = (BindWidget))
		UButton* BlackButton;

	UPROPERTY(meta = (BindWidget))
		UButton* SilverButton;

	class IAMapPlayerControllerInterface* PlayerController;
	
	UFUNCTION()
		void SelectBlack();

	UFUNCTION()
		void SelectSilver();
};
