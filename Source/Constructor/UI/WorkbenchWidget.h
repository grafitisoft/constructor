// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorkbenchWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONSTRUCTOR_API UWorkbenchWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnPlace;

	UPROPERTY(meta=(BindWidget))
	class UButton* BtnSaveBlueprint;
};
