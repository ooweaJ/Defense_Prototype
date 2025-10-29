// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actors/ResourceNode.h"
#include "Components/ResourceComponent.h"
#include "Components/StaticMeshComponent.h"

AResourceNode::AResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetGenerateOverlapEvents(true);
}

void AResourceNode::BeginPlay()
{
	Super::BeginPlay();
}

int32 AResourceNode::Harvest(UResourceComponent* ToInventory)
{
	if (!ToInventory || Amount <= 0)
	{
		return 0;
	}
	const int32 Mined = FMath::Clamp(HarvestPerHit, 0, Amount);
	Amount -= Mined;
	ToInventory->Add(ResourceType, Mined);
	if (Amount <= 0)
	{
		Destroy();
	}
	return Mined;
}


