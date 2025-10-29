// Copyright Epic Games, Inc. All Rights Reserved.

#include "Defense_PrototypePlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Defense_PrototypeCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Managers/SpawnManager.h"
#include "Managers/BuildGridManager.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ADefense_PrototypePlayerController::ADefense_PrototypePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ADefense_PrototypePlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ADefense_PrototypePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ADefense_PrototypePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ADefense_PrototypePlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ADefense_PrototypePlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ADefense_PrototypePlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ADefense_PrototypePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ADefense_PrototypePlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ADefense_PrototypePlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ADefense_PrototypePlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	// Lightweight key binds for prototype (coexist with Enhanced Input)
	InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ADefense_PrototypePlayerController::ToggleBuildMode);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ADefense_PrototypePlayerController::PlaceWall);
	InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ADefense_PrototypePlayerController::Harvest);
	InputComponent->BindKey(EKeys::P, IE_Pressed, this, &ADefense_PrototypePlayerController::StartWave);
}

void ADefense_PrototypePlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ADefense_PrototypePlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void ADefense_PrototypePlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ADefense_PrototypePlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ADefense_PrototypePlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ADefense_PrototypePlayerController::ToggleBuildMode()
{
	if (ADefense_PrototypeCharacter* Ch = Cast<ADefense_PrototypeCharacter>(GetPawn()))
	{
		Ch->ToggleBuildMode(!Ch->bBuildMode);
	}
}

void ADefense_PrototypePlayerController::PlaceWall()
{
	if (ADefense_PrototypeCharacter* Ch = Cast<ADefense_PrototypeCharacter>(GetPawn()))
	{
		if (Ch->bBuildMode)
		{
			Ch->TryPlaceWallAtCursor();
		}
	}
}

void ADefense_PrototypePlayerController::Harvest()
{
	if (ADefense_PrototypeCharacter* Ch = Cast<ADefense_PrototypeCharacter>(GetPawn()))
	{
		Ch->TryHarvestAtCursor();
	}
}

void ADefense_PrototypePlayerController::StartWave()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnManager::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		if (ASpawnManager* SM = Cast<ASpawnManager>(Found[0]))
		{
			SM->StartWave();
		}
	}
}
