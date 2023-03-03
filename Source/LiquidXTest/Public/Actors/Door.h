// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

class UStaticMeshComponent;
class UInteractableComponent;
class UCurveFloat;

UENUM(Blueprintable, BlueprintType)
enum class EDoorStatus :uint8
{
	// The door is Close.
	Close				UMETA(DisplayName = "Close"),

	// The Door is open to the left.
	OpenLeft			UMETA(DisplayName = "OpenLeft"),

	// The Door is Open to the Right.
	OpenRight			UMETA(DisplayName = "OpenRight"),

	// The door is currently opening to left.
	OpeningLeft			UMETA(DisplayName = "OpeningLeft"),

	// The Door is currently opening to right.
	OpeningRight		UMETA(DisplayName = "OpenningRight"),

	// The Door is currently Closing.
	Closing				UMETA(DisplayName = "Closing")
};

UCLASS()
class LIQUIDXTEST_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* Door;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Components)
	UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FRuntimeFloatCurve OpeningDoorCurve;


protected:

	UPROPERTY()
	EDoorStatus CurrentDoorStatus;

	UPROPERTY()
	EDoorStatus TargetDoorStatus;

	UFUNCTION()
	void OnDoorInteracted(AActor* WithActor, AActor* FromActor);

	UPROPERTY()
	float InitialRotation = 0.0f;

	UPROPERTY()
	float TargetRotation = 0.0f;

	UPROPERTY()
	FTimeline OpeningDoorTimeline;

	UPROPERTY()
	UCurveFloat* TimelineCurve;

	FOnTimelineFloat RotateDoorUpdateDelegate{};

	FOnTimelineEvent RotateDoorFinishedDelegate{};

	UFUNCTION()
	void RotateDoorTimelineUpdate(float Alpha);

	UFUNCTION()
	void RotateDoorTimelineOnFinished();

};