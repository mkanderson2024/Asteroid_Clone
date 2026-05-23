// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

class UStaticMesh;
class UStaticMeshComponenet;

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
}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	// Sets direction toward player with a little randomness
	MoveDirection = (PlayerLocation - GetActorLocation()).GetSafeNormal();

	MoveDirection += FVector(
		FMath::RandRange(-0.4f, 0.4f),
		FMath::RandRange(-0.4f, 0.4f),
		0
	);

	MoveDirection.Normalize();
}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + MoveDirection * Speed * DeltaTime);
}

