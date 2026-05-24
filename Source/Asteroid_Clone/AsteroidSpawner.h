// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsteroidSpawner.generated.h"

class AAsteroid;

UCLASS()
class ASTEROID_CLONE_API AAsteroidSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroidSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartSpawning();
	void MaintainAsteroidCount();
	AActor* SpawnAsteroid_Internal();
	FVector GetSpawnPoint() const;
	bool IsVisibleToAnyPlayer(const FVector& Position) const;

	

	FTimerHandle SpawnTimerHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ActiveAsteroids;

public:	
	// Tick might need to be deleted.
	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AAsteroid> AsteroidClass;

	UPROPERTY(EditAnywhere, Category="Spawning")
	float SpawnInterval = 2.0f;

	UPROPERTY(EditAnywhere, Category="Spawning")
	float SpawnDistance = 2500.f;

	UPROPERTY(EditAnywhere, Category="Spawning")
	int32 MaxAsteroids = 10;

	UPROPERTY(EditAnywhere, Category="Spawning")
	int32 MaxAttempts = 10;

};
