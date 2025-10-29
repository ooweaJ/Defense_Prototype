// Copyright Epic Games, Inc. All Rights Reserved.

#include "Managers/SpawnManager.h"
#include "AI/EnemyCharacter.h"
#include "Engine/World.h"

ASpawnManager::ASpawnManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawnManager::StartWave()
{
	SpawnedThisWave = 0;
	TimeSinceLastSpawn = 0.f;
	bWaveActive = true;
}

void ASpawnManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bWaveActive || !Waves.IsValidIndex(CurrentWaveIndex))
	{
		return;
	}
	FWaveEntry& Wave = Waves[CurrentWaveIndex];
	TimeSinceLastSpawn += DeltaSeconds;
	if (SpawnedThisWave < Wave.Count && TimeSinceLastSpawn >= Wave.SpawnInterval)
	{
		TimeSinceLastSpawn = 0.f;
		if (SpawnPoints.Num() > 0 && *Wave.EnemyClass)
		{
			AActor* SpawnAt = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			GetWorld()->SpawnActor<AEnemyCharacter>(Wave.EnemyClass, SpawnAt->GetActorLocation(), SpawnAt->GetActorRotation(), Params);
			SpawnedThisWave++;
		}
	}
	if (SpawnedThisWave >= Wave.Count)
	{
		bWaveActive = false;
		CurrentWaveIndex = FMath::Clamp(CurrentWaveIndex + 1, 0, Waves.Num() - 1);
	}
}


