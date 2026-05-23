// Fill out your copyright notice in the Description page of Project Settings.


#include "AsteroidsGameMode.h"
#include "Fighter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerStart.h"

AAsteroidsGameMode::AAsteroidsGameMode()
{
    DefaultPawnClass = AFighter::StaticClass();
}

void AAsteroidsGameMode::RespawnPlayer(AController* Controller)
{
    if (!Controller) return;

    FTimerHandle RespawnTimer;

    GetWorldTimerManager().SetTimer(
        RespawnTimer,
        [this, Controller]()
        {
            if (!Controller) return;

            // Find a valid PlayerStart in the level
            AActor* Start = ChoosePlayerStart(Controller);

            FVector SpawnLocation = Start
                ? Start->GetActorLocation()
                : FVector(0.f, 0.f, 800.f);

            FRotator SpawnRotation = Start
                ? Start->GetActorRotation()
                : FRotator::ZeroRotator;

            // Spawn the pawn
            APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
                DefaultPawnClass,
                SpawnLocation,
                SpawnRotation
            );

            if (!NewPawn)
            {
                UE_LOG(LogTemp, Error, TEXT("Respawn FAILED: Pawn not spawned"));
                return;
            }

            Controller->Possess(NewPawn);

        },
        RespawnDelay,
        false
    );
}