// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/InteractableInterface.h"
#include "UW_Interaction.generated.h"

/**
 * 
 */
UCLASS()
class LIQUIDXTEST_API UUW_Interaction : public UUserWidget, public IInteractableInterface
{
	GENERATED_BODY()
	
public:

	UFUNCTION()
	void SetInteractionWidgetEnable(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "SetInteractionWidgetEnable"))
	void BP_SetInteractionWidgetEnable(bool bEnable);

	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsInteractionEnable() { return bInteractionEnable; }

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bInteractionEnable = false;

public:

	virtual void SetInteractionEnable_Implementation(bool bEnable) override;

};