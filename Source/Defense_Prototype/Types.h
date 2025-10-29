// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Wood	UMETA(DisplayName = "Wood"),
	Stone	UMETA(DisplayName = "Stone")
};

USTRUCT(BlueprintType)
struct FResourceStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType Type = EResourceType::Wood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;
};

USTRUCT(BlueprintType)
struct FResourceCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceStack> Stacks;
};


