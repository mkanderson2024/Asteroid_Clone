// Fill out your copyright notice in the Description page of Project Settings.


#include "Fighter.h"

// Game Mode
#include "AsteroidsGameMode.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"

// Components
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"

// Engine
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"





// Sets default values
AFighter::AFighter()
{
	// Tags
	Tags.Add("PlayerShip");
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision Sphere
	 CollisionSphere =
	 	CreateDefaultSubobject<USphereComponent>(
			TEXT("CollisionSphere")
		);
	
	RootComponent = CollisionSphere;

	CollisionSphere->InitSphereRadius(40.0f);

	// Ship Mesh
	ShipMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("ShipMesh")
		);

	ShipMesh->SetupAttachment(RootComponent);
	ShipMesh->SetWorldScale3D(FVector(0.2f));
	
	// Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("SpringArm")
	);
	
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1500.0f;   							//Distance from ship
	SpringArm->SetRelativeRotation(FRotator(-70.0f, 0.0f, 0.0f));	//Camera Donward Angle
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 5.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("Camera")
	);
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	// Movement - Possession

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Load Input Assets
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(
    TEXT("/Game/Input/IMC_Fighter.IMC_Fighter")
	);

	if (IMC.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded IMC: %s"),
    	*GetNameSafe(DefaultMappingContext));
		DefaultMappingContext = IMC.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> Move(
		TEXT("/Game/Input/IA_Move.IA_Move")
	);

	if (Move.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded MoveAction: %s"),
    	*GetNameSafe(MoveAction));
		MoveAction = Move.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> Turn(
		TEXT("/Game/Input/IA_Turn.IA_Turn")
	);

	if (Turn.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded TurnAction: %s"),
    	*GetNameSafe(TurnAction));
		TurnAction = Turn.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> Shoot(
		TEXT("/Game/Input/IA_Shoot.IA_Shoot")
	);

	if (Shoot.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded ShootAction: %s"),
   		*GetNameSafe(ShootAction));
		ShootAction = Shoot.Object;
	}

	// Visual Effect
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ThrusterAsset(
    TEXT("/Game/RocketThrusterExhaustFX/FX/NS_RocketExhaust_Afterburn_Jet.NS_RocketExhaust_Afterburn_Jet")
	);

	if (ThrusterAsset.Succeeded())
	{
		ThrusterSystem = ThrusterAsset.Object;
		UE_LOG(LogTemp, Warning, TEXT("Thruster VFX Loaded Successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED to load Thruster VFX"));
	}
	ThrusterVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterVFX"));
	ThrusterVFX->bAutoActivate = false;
	ThrusterVFX->SetupAttachment(ShipMesh, TEXT("ThrusterSocket"));
	ThrusterVFX->SetRelativeScale3D(FVector(2.0f));

	// Audio Effect
	static ConstructorHelpers::FObjectFinder<USoundBase> ThrusterSoundAsset(
    TEXT("/Game/SFX/WAV_ThrusterSound.WAV_ThrusterSound")
	);

	if (ThrusterSoundAsset.Succeeded())
	{
		ThrusterSound = ThrusterSoundAsset.Object;
	}
	ThrusterAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ThrusterAudio"));
	ThrusterAudio->SetupAttachment(ShipMesh, TEXT("ThrusterSocket"));
	ThrusterAudio->bAutoActivate = false;
	ThrusterAudio->SetSound(ThrusterSound);
}

// Called when the game starts or when spawned
void AFighter::BeginPlay()
{
    Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay OK"));
	UE_LOG(LogTemp, Warning, TEXT("ThrusterSystem valid: %s"),
    ThrusterSystem ? TEXT("YES") : TEXT("NO"));

	UE_LOG(LogTemp, Warning, TEXT("Controller = %s"),
	*GetNameSafe(GetController()));
	//MovementComponent->SetUpdatedComponent(RootComponent);
	//MovementComponent->Activate();

    UE_LOG(LogTemp, Warning, TEXT("Fighter Pawn Spawned"));

	// Load mesh
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(
        nullptr,
        TEXT("/Game/ShipMesh/SM_ShipMesh.SM_ShipMesh")
    );

    if (Mesh)
    {
        ShipMesh->SetStaticMesh(Mesh);
        UE_LOG(LogTemp, Warning, TEXT("Mesh loaded successfully in BeginPlay"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FAILED to load mesh in BeginPlay"));
    }

	// Load inhanced inputs
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (DefaultMappingContext)
                {
                    Subsystem->AddMappingContext(DefaultMappingContext, 0);
                }
            }
        }
    }

	if (ThrusterSystem)
	{
		ThrusterVFX->SetAsset(ThrusterSystem);
	}
}

// Called every frame
void AFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Ship Movement using Velocity
	//FVector NewLocation = GetActorLocation() + (Velocity * DeltaTime);
	//SetActorLocation(NewLocation, true);

	// Applies Movement
	AddActorWorldOffset(Velocity * DeltaTime, true);

	// Applies Drag
	Velocity -= Velocity * DragStrength * DeltaTime;

	// Old Drag System
	//Velocity *= FMath::Pow(0.98f, DeltaTime * 60.f);

	if (Velocity.SizeSquared() > MaxSpeed * MaxSpeed)
	{
    Velocity = Velocity.GetSafeNormal() * MaxSpeed;
	}

	if (bIsThrusting)
	{
		if (!ThrusterVFX->IsActive())
		{
			ThrusterVFX->Activate(true);
		}

		if (!ThrusterAudio->IsPlaying())
    	{
        ThrusterAudio->FadeIn(0.05f);
    	}
	}
	else
	{
		if (ThrusterVFX->IsActive())
		{
			ThrusterVFX->Deactivate();
		}

    	ThrusterAudio->FadeOut(0.2f, 0.0f);
	}
}

// Called to bind functionality to input
void AFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent CALLED"));
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInput =
        Cast<UEnhancedInputComponent>(PlayerInputComponent);

    if (!EnhancedInput) return;

    if (MoveAction)
		{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFighter::Move);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AFighter::Move);
		}
    if (TurnAction)
        EnhancedInput->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AFighter::Turn);

    if (ShootAction)
        EnhancedInput->BindAction(ShootAction, ETriggerEvent::Started, this, &AFighter::Shoot);
}

// Possession
void AFighter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
				UE_LOG(LogTemp, Warning, TEXT("Subsystem valid, adding IMC"));
                if (DefaultMappingContext)
                {
                    //Subsystem->AddMappingContext(DefaultMappingContext, 0);
                    UE_LOG(LogTemp, Warning, TEXT("Input Mapping Context added"));
                }
            }
        }
	UE_LOG(LogTemp, Warning, TEXT("Adding Mapping Context..."));
	UE_LOG(LogTemp, Warning, TEXT("MoveAction valid: %s"),
    MoveAction ? TEXT("YES") : TEXT("NO"));

	UE_LOG(LogTemp, Warning, TEXT("TurnAction valid: %s"),
    TurnAction ? TEXT("YES") : TEXT("NO"));

	UE_LOG(LogTemp, Warning, TEXT("ShootAction valid: %s"),
    ShootAction ? TEXT("YES") : TEXT("NO"));
    }
}
// Input
void AFighter::Move(const FInputActionValue& Value)
{
    float Axis = Value.Get<float>();

	bIsThrusting = (FMath::Abs(Axis) > 0.01f);

    // AddMovementInput(GetActorForwardVector(), Axis);
	if (FMath::Abs(Axis) > 0.01f)
	{
		FVector Thrust = GetActorForwardVector() * (Axis * ThrustAcceleration);

		Velocity += Thrust * GetWorld()->GetDeltaSeconds();
	}
}

void AFighter::Turn(const FInputActionValue& Value)
{
    float Axis = Value.Get<float>();
    // AddActorLocalRotation(FRotator(0.f, Axis * 2.0f, 0.f));
	AddActorLocalRotation(FRotator(0.f, Axis * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.f));
}

void AFighter::Shoot(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Pew"));
}

void AFighter::Die()
{
    AController* PawnController = GetController();

    if (PawnController)
    {
        AAsteroidsGameMode* GM = GetWorld()->GetAuthGameMode<AAsteroidsGameMode>();
        if (GM)
        {
            GM->RespawnPlayer(PawnController);
        }
    }

	if (ThrusterVFX->IsActive())
		{
			ThrusterVFX->Deactivate();
		}

	ThrusterAudio->FadeOut(0.2f, 0.0f);

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    DisableInput(nullptr);
}