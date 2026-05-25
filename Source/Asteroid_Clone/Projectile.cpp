// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "UObject/ConstructorHelpers.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("Projectile");

	// Collision Detection
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;

	Collision->InitSphereRadius(10.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	// Niagara Visual
	ProjectileVFX =
		CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileVFX"));

	ProjectileVFX->SetupAttachment(RootComponent);
	ProjectileVFX->SetAutoActivate(true);
	ProjectileVFX->SetRelativeLocation(FVector(15.f, 0.f, 0.f));

	InitialLifeSpan = 15.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();

	ProjectileEffect = LoadObject<UNiagaraSystem>(
			nullptr,
			TEXT("/Game/MixedVFX/Particles/Projectiles/NS_Projectile_01.NS_Projectile_01")
	);

	if (ProjectileEffect)
	{
		ProjectileVFX->SetAsset(ProjectileEffect);
		ProjectileVFX->Activate(true);

		UE_LOG(LogTemp, Warning, TEXT("Projectile VFX Loaded Successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Projectile VFX FAILED to load"));
	}
}


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation =
		GetActorLocation() + (Velocity * DeltaTime);

	SetActorLocation(NewLocation, true);
}

void AProjectile::FireInDirection(const FVector& Direction)
{
    Velocity = Direction.GetSafeNormal() * Speed;
}

void AProjectile::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

	UE_LOG(LogTemp, Warning, TEXT("Projectile Hit: %s"),
    *GetNameSafe(OtherActor));

    Destroy();
}

