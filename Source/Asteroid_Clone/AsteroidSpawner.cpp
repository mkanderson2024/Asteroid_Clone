// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidSpawner.h"

#include "Asteroid.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AAsteroidSpawner::AAsteroidSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = false;
}

// Called when the game starts or when spawned
void AAsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Only the server handles spawning
	if (!HasAuthority())
	{
		return;
	}

	StartSpawning();
}

// Start the repeating spawn timer
void AAsteroidSpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AAsteroidSpawner::MaintainAsteroidCount,
		SpawnInterval,
		true
	);
}

// Maintain max asteroid count
void AAsteroidSpawner::MaintainAsteroidCount()
{
	if (!HasAuthority())
	{
		return;
	}

	// Remove invalid asteroid references
	ActiveAsteroids.RemoveAll([](const TWeakObjectPtr<AActor>& Asteroid)
	{
		return !Asteroid.IsValid();
	});

	int32 CurrentAsteroids = ActiveAsteroids.Num();
	int32 NeededAsteroids = MaxAsteroids - CurrentAsteroids;

	for (int32 i = 0; i < NeededAsteroids; i++)
	{
		AActor* NewAsteroid = SpawnAsteroid_Internal();

		if (NewAsteroid)
		{
			ActiveAsteroids.Add(NewAsteroid);
		}
	}
}

// Attempt to spawn a single asteroid
AActor* AAsteroidSpawner::SpawnAsteroid_Internal()
{
	if (!AsteroidClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return nullptr;
	}

	for (int32 i = 0; i < MaxAttempts; i++)
	{
		FVector SpawnPoint = GetSpawnPoint();

		// Reject spawn if visible to any player
		if (IsVisibleToAnyPlayer(SpawnPoint))
		{
			continue;
		}

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		return World->SpawnActor<AActor>(
			AsteroidClass,
			SpawnPoint,
			FRotator::ZeroRotator,
			Params
		);
	}

	return nullptr;
}

// Generate a spawn point in a ring around the player
FVector AAsteroidSpawner::GetSpawnPoint() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!PlayerPawn)
	{
		return FVector::ZeroVector;
	}

	FVector Origin = PlayerPawn->GetActorLocation();

	FVector Direction = FMath::VRand();
	Direction.Z = 0.f;
	Direction.Normalize();

	return Origin + Direction * SpawnDistance;
}

// Check if a position is visible to any player's camera
bool AAsteroidSpawner::IsVisibleToAnyPlayer(const FVector& Position) const
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return true;
	}

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		if (!PC)
		{
			continue;
		}

		FVector2D ScreenPos;

		if (!PC->ProjectWorldLocationToScreen(Position, ScreenPos))
		{
			continue;
		}

		int32 X, Y;
		PC->GetViewportSize(X, Y);

		if (ScreenPos.X >= 0 && ScreenPos.X <= X &&
			ScreenPos.Y >= 0 && ScreenPos.Y <= Y)
		{
			return true;
		}
	}

	return false;
}