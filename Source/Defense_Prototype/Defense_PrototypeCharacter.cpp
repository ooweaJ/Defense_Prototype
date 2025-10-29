// Copyright Epic Games, Inc. All Rights Reserved.

#include "Defense_PrototypeCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Components/ResourceComponent.h"
#include "Actors/ResourceNode.h"
#include "Managers/BuildGridManager.h"
#include "Actors/WallActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Components/StaticMeshComponent.h"

ADefense_PrototypeCharacter::ADefense_PrototypeCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ResourceComponent = CreateDefaultSubobject<UResourceComponent>(TEXT("ResourceComponent"));
}

void ADefense_PrototypeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// Update build preview location
	if (bBuildMode)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		FHitResult Hit;
		if (TraceUnderCursor(PC, Hit))
		{
			if (!BuildPreviewMesh && *WallClass)
			{
				BuildPreviewMesh = NewObject<UStaticMeshComponent>(this, TEXT("BuildPreview"));
				BuildPreviewMesh->SetupAttachment(RootComponent);
				const AWallActor* DefaultWall = WallClass->GetDefaultObject<AWallActor>();
				if (DefaultWall && DefaultWall->FindComponentByClass<UStaticMeshComponent>())
				{
					UStaticMeshComponent* Src = DefaultWall->FindComponentByClass<UStaticMeshComponent>();
					BuildPreviewMesh->SetStaticMesh(Src->GetStaticMesh());
				}
				BuildPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				BuildPreviewMesh->SetHiddenInGame(false);
				if (BuildPreviewMaterial)
				{
					BuildPreviewMesh->SetMaterial(0, BuildPreviewMaterial);
				}
				BuildPreviewMesh->RegisterComponent();
			}
			if (ABuildGridManager* Grid = FindGridManager())
			{
				const FIntPoint Coord = Grid->WorldToGrid(Hit.Location);
				const FVector Loc = Grid->GridToWorld(Coord);
				if (BuildPreviewMesh)
				{
					BuildPreviewMesh->SetWorldLocation(FVector(Loc.X, Loc.Y, Hit.Location.Z));
					BuildPreviewMesh->SetHiddenInGame(false);
				}
			}
		}
		else if (BuildPreviewMesh)
		{
			BuildPreviewMesh->SetHiddenInGame(true);
		}
	}
}

void ADefense_PrototypeCharacter::ToggleBuildMode(bool bEnable)
{
	bBuildMode = bEnable;
	if (!bBuildMode && BuildPreviewMesh)
	{
		BuildPreviewMesh->SetHiddenInGame(true);
	}
}

 bool ADefense_PrototypeCharacter::TraceUnderCursor(APlayerController* PC, FHitResult& OutHit)
{
	if (!PC)
	{
		return false;
	}
	return PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, OutHit);
}

ABuildGridManager* ADefense_PrototypeCharacter::FindGridManager() const
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildGridManager::StaticClass(), Found);
	return Found.Num() > 0 ? Cast<ABuildGridManager>(Found[0]) : nullptr;
}

bool ADefense_PrototypeCharacter::TryPlaceWallAtCursor()
{
	if (!bBuildMode || !*WallClass)
	{
		return false;
	}
	APlayerController* PC = Cast<APlayerController>(GetController());
	FHitResult Hit;
	if (!TraceUnderCursor(PC, Hit))
	{
		return false;
	}
	ABuildGridManager* Grid = FindGridManager();
	if (!Grid)
	{
		return false;
	}
	const FIntPoint Coord = Grid->WorldToGrid(Hit.Location);
	if (!Grid->CanPlaceAt(Coord))
	{
		return false;
	}
	// Check cost from default object
	const AWallActor* DefaultWall = WallClass->GetDefaultObject<AWallActor>();
	if (DefaultWall)
	{
		if (!ResourceComponent->Consume(DefaultWall->Cost))
		{
			return false;
		}
	}
	return Grid->PlaceWall(Coord, WallClass) != nullptr;
}

bool ADefense_PrototypeCharacter::TryHarvestAtCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	FHitResult Hit;
	if (!TraceUnderCursor(PC, Hit))
	{
		return false;
	}
	if (AResourceNode* Node = Cast<AResourceNode>(Hit.GetActor()))
	{
		return Node->Harvest(ResourceComponent) > 0;
	}
	return false;
}
