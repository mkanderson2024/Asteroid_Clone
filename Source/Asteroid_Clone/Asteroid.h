// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

#include "Asteroid.generated.h" // Must always be last


UCLASS()
class ASTEROID_CLONE_API AAsteroid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Mesh component
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category="Asteroid")
	UStaticMesh* AsteroidMesh;

	// Movement variables
	UPROPERTY()
	FVector Velocity;

	UPROPERTY(EditAnywhere, Category="Asteroid")
	float Speed = 300.f;

	// Rotation Variables
	FVector RotationAxis;
	float RotationSpeed = 60.f;
};
