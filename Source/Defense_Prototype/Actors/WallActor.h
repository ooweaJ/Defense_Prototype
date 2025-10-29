// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types.h"
#include "WallActor.generated.h"

class UStaticMeshComponent;
class UHealthComponent;

UCLASS()
class AWallActor : public AActor
{
	GENERATED_BODY()

public:
	AWallActor();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	FResourceCost Cost;

	UFUNCTION()
	void HandleDeath();
};


