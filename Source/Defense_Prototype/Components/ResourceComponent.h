// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types.h"
#include "ResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChanged, EResourceType, Type, int32, NewAmount);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TMap<EResourceType, int32> ResourceAmounts;

	UPROPERTY(BlueprintAssignable)
	FOnResourceChanged OnResourceChanged;

	UFUNCTION(BlueprintCallable)
	int32 Get(EResourceType Type) const;

	UFUNCTION(BlueprintCallable)
	void Add(EResourceType Type, int32 Delta);

	UFUNCTION(BlueprintCallable)
	bool CanAfford(const FResourceCost& Cost) const;

	UFUNCTION(BlueprintCallable)
	bool Consume(const FResourceCost& Cost);
};


