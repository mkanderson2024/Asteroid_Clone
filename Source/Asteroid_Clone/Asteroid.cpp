// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

class UStaticMesh;
class UStaticMeshComponent;

// Sets default values
AAsteroid::AAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetSimulatePhysics(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Game/AsteroidMesh/Asteroid.Asteroid")
	);

	if (MeshAsset.Succeeded())
	{
		AsteroidMesh = MeshAsset.Object;
		Mesh->SetStaticMesh(AsteroidMesh);
	}

	RotationAxis = FVector::ZeroVector;
	RotationSpeed = 60.f;
}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	// Sets direction toward player with a little randomness
	FVector MoveDirection = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	FVector RandomOffset = FVector(
		FMath::RandRange(-0.4f, 0.4f),
		FMath::RandRange(-0.4f, 0.4f),
		0
	);

	FVector FinalDirection = (MoveDirection + RandomOffset).GetSafeNormal();

	Velocity = FinalDirection * Speed;

	// Rotation with a little randomness
	RotationAxis = FMath::VRand().GetSafeNormal();
	RotationSpeed = FMath::FRandRange(30.f, 180.f);
}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;
	
	FVector MoveDirection =
		(PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	// Slgith attaction to player ship
	Velocity += MoveDirection * 10.f * DeltaTime;

	// Moves asteroid
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);

	// Tumbling effect
	AddActorWorldRotation(
		FQuat(RotationAxis, FMath::DegreesToRadians(RotationSpeed * DeltaTime))
	);
}

