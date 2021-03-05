// Fill out your copyright notice in the Description page of Project Settings.

#include "AMapFPSGameMode.h"
#include "Camera/CameraActor.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerFPS/MultiplayerFPSCharacter.h"
#include "MultiplayerFPS/Public/AMapFPSPlayerController.h"

AAMapFPSGameMode::AAMapFPSGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/_Game/Blueprints/Characters/BP_CharacterBlank"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/_Game/Blueprints/Controllers/BP_AMapPlayerController"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;
}

void AAMapFPSGameMode::SpawnPlayerCharacter(APlayerController* NewPlayer, bool bBlack)
{
	AActor* SpawnedCharacter;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (bBlack)
	{
		FTransform SpawnTransform = BlackSpawnPoints[FMath::RandRange(0, BlackSpawnPoints.Num() - 1)]->GetActorTransform();
		SpawnedCharacter = GetWorld()->SpawnActor(BlackCharacter, &SpawnTransform, SpawnParams);
	}
	else
	{
		FTransform SpawnTransform = SilverSpawnPoints[FMath::RandRange(0, SilverSpawnPoints.Num() - 1)]->GetActorTransform();
		SpawnedCharacter = GetWorld()->SpawnActor(SilverCharacter, &SpawnTransform, SpawnParams);
	}

	NewPlayer->Possess(Cast<APawn>(SpawnedCharacter));
	
	NewPlayer->SetInputMode(FInputModeGameOnly());
	NewPlayer->bShowMouseCursor = false;
}

void AAMapFPSGameMode::SetViewToLoginCamera(APlayerController* NewPlayer)
{
	NewPlayer->PlayerCameraManager->SetManualCameraFade(1.f, FLinearColor::Black, true);

	FTimerHandle SetViewToCamDelayHandle;
	FTimerDelegate SetViewToCamDelayDelegate = FTimerDelegate::CreateUObject(this, &AAMapFPSGameMode::SetViewToLoginCamera_Delayed, NewPlayer);

	GetWorld()->GetTimerManager().SetTimer(SetViewToCamDelayHandle, SetViewToCamDelayDelegate, 1.f, false);
}

void AAMapFPSGameMode::SetViewToLoginCamera_Delayed(APlayerController* NewPlayer)
{
	NewPlayer->PlayerCameraManager->SetViewTarget(LoginCam);
	NewPlayer->PlayerCameraManager->StartCameraFade(1.f, 0.f, 0.25f, FLinearColor::Black, true);

	AAMapFPSPlayerController* FPSController = Cast<AAMapFPSPlayerController>(NewPlayer);
	if (FPSController)
	{
		FPSController->DisplayTeamSelectionWidget();
		NewPlayer->SetInputMode(FInputModeUIOnly());
		NewPlayer->bShowMouseCursor = true;
	}
}

void AAMapFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName(TEXT("LoginCam")), Cameras);
	LoginCam = Cameras[0];

	UGameplayStatics::GetAllActorsOfClassWithTag(this, ATargetPoint::StaticClass(), FName(TEXT("Black")), BlackSpawnPoints);
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ATargetPoint::StaticClass(), FName(TEXT("Silver")), SilverSpawnPoints);
}

void AAMapFPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NewPlayer->GetPawn()->Destroy();
	SetViewToLoginCamera(NewPlayer);
}