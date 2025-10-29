// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actors/WallActor.h"
#include "Components/HealthComponent.h"
#include "Components/StaticMeshComponent.h"

AWallActor::AWallActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void AWallActor::BeginPlay()
{
	Super::BeginPlay();
	if (Health)
	{
		Health->OnDeath.AddDynamic(this, &AWallActor::HandleDeath);
	}
}

void AWallActor::HandleDeath()
{
	Destroy();
}


