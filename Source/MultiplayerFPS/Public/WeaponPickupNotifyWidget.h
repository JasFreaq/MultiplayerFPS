// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponPickupNotifyWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API UWeaponPickupNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetNotifyText(const FString& Text);
	
private:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* NotifyText;
};
