// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class UInteractableComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIQUIDXTEST_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Interface")
	void RegisterInteractableComponent(UInteractableComponent* InComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Interface")
	void UnRegisterInteractableComponent(UInteractableComponent* InComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Interface")
	void SetInteractionEnable(bool bEnable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Interface")
	void OnInteractBody(AActor* FromActor);

};