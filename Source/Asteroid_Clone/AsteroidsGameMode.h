// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AsteroidsGameMode.generated.h"

UCLASS()
class ASTEROID_CLONE_API AAsteroidsGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    AAsteroidsGameMode();

    void RespawnPlayer(AController* Controller);

protected:

    UPROPERTY(EditDefaultsOnly, Category="Respawn")
    TSubclassOf<APawn> PlayerPawnClass;

    UPROPERTY(EditDefaultsOnly, Category="Respawn")
    float RespawnDelay = 3.0f;

    FVector GetRespawnLocation() const;
};