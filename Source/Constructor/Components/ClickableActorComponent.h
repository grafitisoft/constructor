﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClickableActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CONSTRUCTOR_API UClickableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UClickableActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Clickable)
	UMaterialInterface* SelectedMaterial;

	UPROPERTY()
	UMaterialInterface* DeSelectedMaterial;

private:
	UFUNCTION()
	void OnClicked(AActor* InActorClicked, FKey InButtonPressed);

	UFUNCTION()
	void OnCursorOver(AActor* InActorCursorOver);

	UFUNCTION()
	void OnCursorExit(AActor* InActorCursorOver);

	uint32 bIsSelected:1;
};
