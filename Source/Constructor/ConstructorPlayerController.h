// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/WorkbenchActor.h"
#include "GameFramework/PlayerController.h"
#include "ConstructorPlayerController.generated.h"

UCLASS()
class CONSTRUCTOR_API AConstructorPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AConstructorPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category=ConstructorController)
	UConstructionActorComponent* ConstructionComp;
	
	UPROPERTY()
	AWorkbenchActor* CurrentWorkbench;
	
	UFUNCTION()
	void OnWorkbenchActivationStatusChanged(AWorkbenchActor* InWorkbench, bool bIsActive);

	UFUNCTION()
	void BeginBlueprintPlace();

	UFUNCTION()
	void EndBlueprintPlace();

	UFUNCTION()
	void SwitchPlacingBlueprint(float InAxisValue);

	uint32 bIsPlacingBlueprint:1;

	int CurrentBlueprintIndex;
};
