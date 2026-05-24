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
	AActor* GetClosestPlayer() const;

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
	float MaxSpeed = 300.f;

	UPROPERTY()
	float InitialSpeed = 0.f;

	// Respawn/Redirect variables
	UPROPERTY(EditAnywhere, Category="Asteroid|Behavior")
	float DespawnDistance = 8000.f;

	UPROPERTY(EditAnywhere, Category="Asteroid|Behavior")
	float ReturnDistance = 5000.f;

	UPROPERTY(EditAnywhere, Category="Asteroid|Behavior")
	float ReturnForce = 1.5f;

	bool IsVisibleToAnyPlayer() const;

	// Rotation variables
	FVector RotationAxis;
	float RotationSpeed = 60.f;

	UPROPERTY()
	float MinSpeed = 150.f;

	UFUNCTION()

	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	FTimerHandle InitPhysicsHandle;
	void InitPhysics();
};
