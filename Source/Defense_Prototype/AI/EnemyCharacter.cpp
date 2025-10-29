// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/EnemyCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Actors/WallActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

    AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
    AttackRange->SetupAttachment(RootComponent);
    AttackRange->InitSphereRadius(150.f);
    AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnRangeBegin);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnRangeEnd);
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!CurrentTarget.IsValid())
	{
		FindTarget();
	}
    if (CurrentTarget.IsValid())
	{
        const float Dist = FVector::Dist(CurrentTarget->GetActorLocation(), GetActorLocation());
        if (Dist <= AttackRange->GetScaledSphereRadius())
        {
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC)
            {
                AIC->StopMovement();
            }
            if (!GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
            {
                GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyCharacter::ApplyAttack, AttackInterval, true, 0.f);
            }
        }
        else
        {
            GetWorldTimerManager().ClearTimer(AttackTimerHandle);
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC)
            {
                AIC->MoveToActor(CurrentTarget.Get(), AttackRange->GetScaledSphereRadius() * 0.8f, true, true, true, 0, true);
            }
        }
	}
}

void AEnemyCharacter::FindTarget()
{
	// Prefer nearest wall, fallback to player
	TArray<AActor*> Walls;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallActor::StaticClass(), Walls);
	AActor* Best = nullptr;
	float BestDist = TNumericLimits<float>::Max();
	for (AActor* W : Walls)
	{
		const float D = FVector::DistSquared(W->GetActorLocation(), GetActorLocation());
		if (D < BestDist)
		{
			Best = W;
			BestDist = D;
		}
	}
	if (!Best)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		Best = PlayerPawn;
	}
	CurrentTarget = Best;
}

void AEnemyCharacter::OnRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AWallActor::StaticClass()))
	{
		CurrentTarget = OtherActor;
		if (!GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
		{
			GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyCharacter::ApplyAttack, AttackInterval, true, 0.f);
		}
	}
}

void AEnemyCharacter::OnRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor == CurrentTarget.Get())
	{
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		CurrentTarget = nullptr;
	}
}

void AEnemyCharacter::ApplyAttack()
{
	if (AWallActor* Wall = Cast<AWallActor>(CurrentTarget.Get()))
	{
		if (Wall->IsActorBeingDestroyed())
		{
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
			CurrentTarget = nullptr;
			return;
		}
		if (Wall->Health)
		{
			Wall->Health->ApplyDamage(AttackDamage);
		}
	}
}


