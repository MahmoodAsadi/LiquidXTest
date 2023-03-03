// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InteractableComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>("DoorFrame");
	RootComponent = DoorFrame;
	DoorFrame->SetCollisionProfileName("BlockAll");

	Door = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	Door->SetupAttachment(RootComponent);
	Door->SetCollisionProfileName("BlockAll");

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>("InteractableComponent");
	InteractableComponent->SetupAttachment(Door);

	FKeyHandle KeyHandle1;
	FKeyHandle KeyHandle2;
	OpeningDoorCurve.GetRichCurve()->AddKey(0.0f, 0.0f, false, KeyHandle1);
	OpeningDoorCurve.GetRichCurve()->SetKeyInterpMode(KeyHandle1, RCIM_Cubic);

	OpeningDoorCurve.GetRichCurve()->AddKey(0.6f, 1.0f, false, KeyHandle2);
	OpeningDoorCurve.GetRichCurve()->SetKeyInterpMode(KeyHandle2, RCIM_Cubic);

	RotateDoorUpdateDelegate.BindDynamic(this, &ADoor::RotateDoorTimelineUpdate);
	RotateDoorFinishedDelegate.BindDynamic(this, &ADoor::RotateDoorTimelineOnFinished);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	TimelineCurve = NewObject<UCurveFloat>(this);
	TimelineCurve->FloatCurve = *OpeningDoorCurve.GetRichCurve();
	OpeningDoorTimeline.AddInterpFloat(TimelineCurve, RotateDoorUpdateDelegate, "Alpha");
	OpeningDoorTimeline.SetTimelineFinishedFunc(RotateDoorFinishedDelegate);

	InteractableComponent->OnInteract.AddDynamic(this, &ADoor::OnDoorInteracted);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OpeningDoorTimeline.TickTimeline(DeltaTime);
}

void ADoor::OnDoorInteracted(AActor* WithActor, AActor* FromActor)
{
	if (CurrentDoorStatus == EDoorStatus::OpeningRight || CurrentDoorStatus == EDoorStatus::OpeningLeft || CurrentDoorStatus == EDoorStatus::Closing)
	{
		// Do nothing if door is already opening or closing.
		return;
	}
	
	bool bOpenToRight = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), FromActor->GetActorLocation()).X < 0.0f;
	InitialRotation = Door->GetRelativeRotation().Yaw;

	switch (CurrentDoorStatus)
	{
		case EDoorStatus::Close:

			TargetRotation = bOpenToRight ? 90.0f : -90.0f;
			CurrentDoorStatus = bOpenToRight ? EDoorStatus::OpeningRight : EDoorStatus::OpeningLeft;
			TargetDoorStatus = bOpenToRight ? EDoorStatus::OpenRight : EDoorStatus::OpenLeft;

		break;

		case EDoorStatus::OpenLeft:
		case EDoorStatus::OpenRight:

			TargetDoorStatus = EDoorStatus::Close;
			CurrentDoorStatus = EDoorStatus::Closing;
			TargetRotation = 0.0f;

		break;
	}
	
	InteractableComponent->SetActive(false);
	OpeningDoorTimeline.PlayFromStart();
}

void ADoor::RotateDoorTimelineUpdate(float Alpha)
{
	Door->SetRelativeRotation(FRotator(0.0f, FMath::Lerp<float>(InitialRotation, TargetRotation, Alpha), 0.0f));
}

void ADoor::RotateDoorTimelineOnFinished()
{
	InteractableComponent->SetActive(true);
	CurrentDoorStatus = TargetDoorStatus;
}