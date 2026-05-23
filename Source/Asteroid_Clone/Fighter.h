// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "Fighter.generated.h" // Must always be last

class USphereComponent;
class UStaticMeshComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UNiagaraComponent;
class UNiagaraSystem;


UCLASS()
class ASTEROID_CLONE_API AFighter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFighter();

	void Move(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);

	// Death and Resapwn
	void Die();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Pawn Possession
	virtual void PossessedBy(AController* NewController) override;

	// Components list
	private:

		UPROPERTY(VisibleAnywhere)
		USphereComponent* CollisionSphere;

		UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ShipMesh;

		//Old Movement Control for Floating Pawn movement
		//UPROPERTY(VisibleAnywhere)
		//UFloatingPawnMovement* MovementComponent;

		// Movement Controls
		UPROPERTY(EditAnywhere, Category="Movement")
		float ThrustAcceleration = 2500.f; // Low value pushes in direction less, high value pushes in direction harder

		UPROPERTY(EditAnywhere, Category="Movement")
		float RotationSpeed = 220.f;

		UPROPERTY(EditAnywhere, Category="Movement")
		float MaxSpeed = 4000.f;

		UPROPERTY(EditAnywhere, Category="Movement")
		float DragStrength = 0.5f; // Low Value long drift, high value quick stop A good neutral is 2.0f

		FVector Velocity = FVector::ZeroVector;

		UPROPERTY(VisibleAnywhere)
		USpringArmComponent* SpringArm;

		UPROPERTY(VisibleAnywhere)
		UCameraComponent* Camera;

		UPROPERTY(EditDefaultsOnly, Category="Input")
		UInputMappingContext* DefaultMappingContext;

		UPROPERTY(EditDefaultsOnly, Category="Input")
		UInputAction* MoveAction;

		UPROPERTY(EditDefaultsOnly, Category="Input")
		UInputAction* TurnAction;

		UPROPERTY(EditDefaultsOnly, Category="Input")
		UInputAction* ShootAction;

		UPROPERTY(VisibleAnywhere, Category = "VFX")
		UNiagaraComponent* ThrusterVFX;

		UPROPERTY(EditDefaultsOnly, Category = "VFX")
		UNiagaraSystem* ThrusterSystem;

		bool bIsThrusting = false;

		UPROPERTY(VisibleAnywhere)
		UAudioComponent* ThrusterAudio;

		UPROPERTY(EditDefaultsOnly, Category = "Audio")
		USoundBase* ThrusterSound;
};
