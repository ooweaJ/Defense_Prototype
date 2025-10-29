// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::Initialize(float InMaxHealth)
{
	MaxHealth = FMath::Max(1.f, InMaxHealth);
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth);
}

void UHealthComponent::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f || CurrentHealth <= 0.f)
	{
		return;
	}
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);
	if (CurrentHealth <= 0.f)
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.f || CurrentHealth <= 0.f)
	{
		return;
	}
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);
}


