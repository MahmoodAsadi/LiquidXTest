// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/JetPackCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AJetPackCharacter::AJetPackCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetAbsolute(true, false, true);
	SpringArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ThrustParticle = CreateDefaultSubobject<UParticleSystemComponent>("ThrustParticle");
	ThrustParticle->SetupAttachment(RootComponent);
	ThrustParticle->SetAutoActivate(false);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

}

// Called when the game starts or when spawned
void AJetPackCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AJetPackCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Movement.
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f));

	FVector BloomPosition = GetActorLocation();
	BloomPosition.Z = FixHeightPosition;
	SpringArm->SetWorldLocation(BloomPosition);
}

// Called to bind functionality to input
void AJetPackCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Thrusting
		EnhancedInputComponent->BindAction(ThrustAction, ETriggerEvent::Started, this, &AJetPackCharacter::StartThrust);
		EnhancedInputComponent->BindAction(ThrustAction, ETriggerEvent::Completed, this, &AJetPackCharacter::StopThrust);
	}
}

void AJetPackCharacter::StartThrust()
{
	Jump();

	ThrustParticle->SetActive(true);
}

void AJetPackCharacter::StopThrust()
{
	StopJumping();
	// Making Infinite jump.
	JumpMaxCount++;
	ThrustParticle->SetActive(false);
}

void AJetPackCharacter::Landed(const FHitResult& Hit)
{
	// Reset Jump count.
	JumpMaxCount = 1;
}
