// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/ResourceComponent.h"

UResourceComponent::UResourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ResourceAmounts.Add(EResourceType::Wood, 0);
	ResourceAmounts.Add(EResourceType::Stone, 0);
}

int32 UResourceComponent::Get(EResourceType Type) const
{
	if (const int32* Found = ResourceAmounts.Find(Type))
	{
		return *Found;
	}
	return 0;
}

void UResourceComponent::Add(EResourceType Type, int32 Delta)
{
	int32& Value = ResourceAmounts.FindOrAdd(Type);
	Value = FMath::Max(0, Value + Delta);
	OnResourceChanged.Broadcast(Type, Value);
}

bool UResourceComponent::CanAfford(const FResourceCost& Cost) const
{
	for (const FResourceStack& Stack : Cost.Stacks)
	{
		if (Get(Stack.Type) < Stack.Amount)
		{
			return false;
		}
	}
	return true;
}

bool UResourceComponent::Consume(const FResourceCost& Cost)
{
	if (!CanAfford(Cost))
	{
		return false;
	}
	for (const FResourceStack& Stack : Cost.Stacks)
	{
		Add(Stack.Type, -Stack.Amount);
	}
	return true;
}


