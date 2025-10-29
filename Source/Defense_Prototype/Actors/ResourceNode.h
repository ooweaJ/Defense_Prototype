// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types.h"
#include "ResourceNode.generated.h"

class UStaticMeshComponent;
class UResourceComponent;

UCLASS()
class AResourceNode : public AActor
{
	GENERATED_BODY()

public:
	AResourceNode();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	EResourceType ResourceType = EResourceType::Wood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 Amount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 HarvestPerHit = 1;

	UFUNCTION(BlueprintCallable)
	int32 Harvest(UResourceComponent* ToInventory);
};


