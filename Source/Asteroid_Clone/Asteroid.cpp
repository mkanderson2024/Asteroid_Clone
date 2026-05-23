// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AAsteroid::AAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetSimulatedPhysics(false);

	static (ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEST("/Game/AsteroidMesh/Asteroid.Asteroid")
	));

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
	
}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

