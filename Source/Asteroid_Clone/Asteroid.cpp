// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Fighter.h"
#include "GameConstants.h"

class UStaticMesh;
class UStaticMeshComponent;

// Sets default values
AAsteroid::AAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh Settings
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Set physics
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->BodyInstance.bUseCCD = true;

	//R Remove Damping
	Mesh->SetLinearDamping(0.f);
	Mesh->SetAngularDamping(0.f);

	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	Mesh->BodyInstance.bLockZTranslation = true;

	Mesh->OnComponentHit.AddDynamic(this, &AAsteroid::OnHit);

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

    GetWorldTimerManager().SetTimer(
        InitPhysicsHandle,
        this,
        &AAsteroid::InitPhysics,
        0.1f,
        false
    );
}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!Mesh) return;

    FVector MyLocation = GetActorLocation();
    AActor* Player = GetClosestPlayer();

    if (!Player)
        return;

    float DistSq = FVector::DistSquared(Player->GetActorLocation(), MyLocation);

    // Despawn when outside of range
    if (DistSq > FMath::Square(DespawnDistance))
    {
        Destroy();
        return;
    }

	// Nudge back to nearest player
    if (!IsVisibleToAnyPlayer() && DistSq > FMath::Square(ReturnDistance))
    {
        FVector ToPlayer = (Player->GetActorLocation() - MyLocation).GetSafeNormal();

        FVector CurrentVel = Mesh->GetPhysicsLinearVelocity();

        FVector TargetVel = ToPlayer * InitialSpeed;

        FVector NewVel = FMath::Lerp(CurrentVel, TargetVel, ReturnForce * DeltaTime);

        Mesh->SetPhysicsLinearVelocity(NewVel);

		Mesh->WakeAllRigidBodies();

        return;
    }
}

void AAsteroid::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
	{
	if (!OtherActor)
			return;

		// Player collision
		if (OtherActor->ActorHasTag("PlayerShip"))
		{
			if (AFighter* Fighter = Cast<AFighter>(OtherActor))
			{
				Fighter->Die();
			}

			Destroy();
			return;
		}

		// Projectile collision (you'll likely add later)
		if (OtherActor->ActorHasTag("Projectile"))
		{
			Destroy();
			return;
		}

}

void AAsteroid::InitPhysics()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	// Moves toward player
	FVector MoveDirection = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	// Creates a slight spread of the asteroids
	FVector RandomOffset = FVector(
		FMath::RandRange(-0.4f, 0.4f),
		FMath::RandRange(-0.4f, 0.4f),
		0.f
	);

	FVector FinalDirection = (MoveDirection + RandomOffset).GetSafeNormal();

	// Inital Speed
	InitialSpeed = FMath::FRandRange(MinSpeed, MaxSpeed);

	Mesh->SetPhysicsLinearVelocity(FinalDirection * InitialSpeed);

	RotationAxis = FMath::VRand().GetSafeNormal();
	RotationSpeed = FMath::FRandRange(80.f, 300.f);

	Mesh->SetPhysicsAngularVelocityInDegrees(RotationAxis * RotationSpeed);

	Mesh->WakeAllRigidBodies();
}

AActor* AAsteroid::GetClosestPlayer() const
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    AActor* Closest = nullptr;
    float BestDistSq = FLT_MAX;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC || !PC->GetPawn()) continue;

        AActor* Player = PC->GetPawn();
        float DistSq = FVector::DistSquared(Player->GetActorLocation(), GetActorLocation());

        if (DistSq < BestDistSq)
        {
            BestDistSq = DistSq;
            Closest = Player;
        }
    }

    return Closest;
}

bool AAsteroid::IsVisibleToAnyPlayer() const
{
    UWorld* World = GetWorld();
    if (!World) return false;

    FVector Position = GetActorLocation();

    for (FConstPlayerControllerIterator Iteration = World->GetPlayerControllerIterator(); Iteration; ++Iteration)
    {
        APlayerController* PC = Iteration->Get();
        if (!PC) continue;

        FVector CameraLoc;
        FRotator CameraRot;
        PC->GetPlayerViewPoint(CameraLoc, CameraRot);

        FVector ToPoint = (Position - CameraLoc).GetSafeNormal();
        float Dot = FVector::DotProduct(CameraRot.Vector(), ToPoint);

        if (Dot > 0.2f)
        {
            return true;
        }
    }

    return false;
}