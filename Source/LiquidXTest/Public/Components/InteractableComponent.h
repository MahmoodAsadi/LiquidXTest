// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Interface/InteractableInterface.h"
#include "InteractableComponent.generated.h"

class USphereComponent;
class UUW_Interaction;
class UWidgetComponent;
class UMaterialInterface;
class ACharacter;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteract, AActor*, WithActor, AActor*, FromActor);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LIQUIDXTEST_API UInteractableComponent : public USceneComponent, public IInteractableInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void Activate(bool bReset = true) override;
	virtual void Deactivate() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region ExposedVariables

public:

	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteract;

	// Radius which player can interact with this component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Info")
	float InteractionRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Info")
	float AllowedInteractionAngle = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Info")
	FName InteractionCollisionProfileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Info")
	float WidgetZOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Info")
	TSubclassOf<UUW_Interaction> InteractionWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Info")
	UMaterialInterface* WidgetMaterial;

#pragma endregion ExposedVariables
		

protected:

	UPROPERTY(Transient)
	ACharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Interactable Datas")
	USphereComponent* InteractionCollision = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Interactable Datas")
	UWidgetComponent* InteractionWidgetComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Interactable Datas")
	UUW_Interaction* InteractionUserWidget = nullptr;

	UPROPERTY(Transient)
	bool bPlayerInRange = false;

	UPROPERTY(Transient)
	bool bIsEnableToInteract = false;

	UFUNCTION()
	void OnInteractionCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	bool CanInteract();

public:

	virtual void SetInteractionEnable_Implementation(bool bEnable) override;

	virtual void OnInteractBody_Implementation(AActor* FromActor) override;

};