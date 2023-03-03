// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractableComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Widgets/UW_Interaction.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bAutoActivate = true;
}

// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetOwner(), 0);

	// Create and setup Interaction collision to enable interaction when player reaches it.
	InteractionCollision = NewObject<USphereComponent>(GetOwner());
	InteractionCollision->RegisterComponent();

	// Attach Interaction collision to intractable component.
	InteractionCollision->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	InteractionCollision->SetSphereRadius(InteractionRadius, true);
	
	if (InteractionCollisionProfileName.IsNone())
	{
		// Setup collision manually if profile name is not valid.
		InteractionCollision->SetCollisionProfileName("Custom");
		InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else
	{
		// Use user setup collision name.
		InteractionCollision->SetCollisionProfileName(InteractionCollisionProfileName);
	}

	// Bind Interaction collision begin overlap and end overlap delegates to our function.
	InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::OnInteractionCollisionBeginOverlap);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::OnInteractionCollisionEndOverlap);

	if (InteractionWidgetClass != nullptr)
	{
		// Create and Setup Widget component.
		InteractionWidgetComponent = NewObject<UWidgetComponent>(GetOwner());
		InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClass);
		InteractionWidgetComponent->RegisterComponent();
		InteractionWidgetComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		InteractionWidgetComponent->SetDrawSize(FVector2D(32.0f));
		InteractionWidgetComponent->SetMaterial(0, WidgetMaterial);
		InteractionWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, WidgetZOffset));

		InteractionUserWidget = Cast<UUW_Interaction>(InteractionWidgetComponent->GetWidget());
	}
}

void UInteractableComponent::Activate(bool bReset /*= true*/)
{
	Super::Activate(bReset);

	if (InteractionCollision == nullptr)
	{
		return;
	}

	if (InteractionCollisionProfileName.IsNone())
	{
		// Setup collision manually if profile name is not valid.
		InteractionCollision->SetCollisionProfileName("Custom");
		InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else
	{
		// Use user setup collision name.
		InteractionCollision->SetCollisionProfileName(InteractionCollisionProfileName);
	}
}

void UInteractableComponent::Deactivate()
{
	Super::Deactivate();

	if (InteractionCollision == nullptr)
	{
		return;
	}

	InteractionCollision->SetCollisionProfileName("NoCollision", true);

	if (bIsEnableToInteract)
	{
		bIsEnableToInteract = false;
		IInteractableInterface::Execute_UnRegisterInteractableComponent(PlayerCharacter, this);
		IInteractableInterface::Execute_SetInteractionEnable(InteractionUserWidget, false);
	}
}

// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
	{
		return;
	}

	if (!bPlayerInRange)
	{
		return;
	}

	bool bCurrentPossiblity = CanInteract();

	if (bIsEnableToInteract != bCurrentPossiblity)
	{
		bIsEnableToInteract = bCurrentPossiblity;

		if (bIsEnableToInteract)
		{
			IInteractableInterface::Execute_RegisterInteractableComponent(PlayerCharacter, this);
		}
		else
		{
			IInteractableInterface::Execute_UnRegisterInteractableComponent(PlayerCharacter, this);
		}
	}
}

void UInteractableComponent::OnInteractionCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsActive())
	{
		return;
	}

	if (OtherActor == PlayerCharacter)
	{
		bPlayerInRange = true;
	}
}

void UInteractableComponent::OnInteractionCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsActive())
	{
		return;
	}

	if (OtherActor == PlayerCharacter)
	{
		bPlayerInRange = false;
		bIsEnableToInteract = false;
		IInteractableInterface::Execute_UnRegisterInteractableComponent(PlayerCharacter, this);
	}
}

bool UInteractableComponent::CanInteract()
{
	if (!IsActive())
	{
		return false;
	}

	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetOwner(), 0);
	
	// Calculate dot with camera direction and direction to interactable component.
	float Dot = FVector::DotProduct(PlayerCameraManager->GetActorForwardVector(), (GetComponentLocation() - PlayerCameraManager->GetCameraLocation()).GetSafeNormal());
	bool bLookingAtInteractable = FMath::Cos(FMath::DegreesToRadians(AllowedInteractionAngle)) < Dot;

	if (!bLookingAtInteractable)
	{
		// If not looking to the interactable ignore.
		return false;
	}

	FHitResult TraceHitResult;
	TArray<AActor*> IgnoringActors;
	IgnoringActors.Add(GetOwner());
	IgnoringActors.Add(PlayerCharacter);

	// Performing Trace, If trace hitted nothing, it means there is nothing in the way so we can interact.
	bool bTraceHitted = UKismetSystemLibrary::LineTraceSingle(GetOwner(), PlayerCameraManager->GetCameraLocation(), GetComponentLocation(), 
		TraceTypeQuery1, true, IgnoringActors, EDrawDebugTrace::None, TraceHitResult, true);

	return !bTraceHitted;
}

void UInteractableComponent::SetInteractionEnable_Implementation(bool bEnable)
{
	if (!IsActive())
	{
		return;
	}

	IInteractableInterface::Execute_SetInteractionEnable(InteractionUserWidget, bEnable);
}

void UInteractableComponent::OnInteractBody_Implementation(AActor* FromActor)
{
	if (!IsActive())
	{
		return;
	}

	if (OnInteract.IsBound())
	{
		OnInteract.Broadcast(GetOwner(), FromActor);
	}
}