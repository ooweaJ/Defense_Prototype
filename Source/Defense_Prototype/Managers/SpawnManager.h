// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

class AEnemyCharacter;

USTRUCT(BlueprintType)
struct FWaveEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval = 1.0f;
};

UCLASS()
class ASpawnManager : public AActor
{
	GENERATED_BODY()

public:
	ASpawnManager();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	TArray<FWaveEntry> Waves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	int32 CurrentWaveIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	TArray<AActor*> SpawnPoints;

	UFUNCTION(BlueprintCallable)
	void StartWave();

protected:
	virtual void BeginPlay() override;

private:
	float TimeSinceLastSpawn = 0.f;
	int32 SpawnedThisWave = 0;
	bool bWaveActive = false;
};


