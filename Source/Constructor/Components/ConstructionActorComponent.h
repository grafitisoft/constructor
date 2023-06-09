﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConstructionActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConstructionObjectPlacedDelegate, class UConstructionActorComponent*, InConstructionActor, AActor *, InNewActor);

/**
 * This component handles the main placement logic. Two different begins mode exists:
 * One functioning via a StaticMesh, to place and build Blueprints, using Workbench trace channel (ECC_GameTraceChannel2)
 * One functioning via a Blueprint Actor, to place Blueprints using Floor trace channel (ECC_GameTraceChannel2)
 * Placeable Actor and Clickable Actor components are added/removed ad runtime when needed to support any actor for further extensions
 * @todo we may bind to inputs for rotation and scale in the future. Now the owner actors communicate handling input. Engine crashes when accessing owners InputComponent
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CONSTRUCTOR_API UConstructionActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UConstructionActorComponent();

	UFUNCTION()
	void SwitchPlacingConstructMesh(UStaticMesh* InNewMesh) const;

	UFUNCTION()
	void SwitchPlacingBlueprint(class ABlueprintActor* InNewActor);
	
	UFUNCTION()
	void BeginPlacement(UStaticMesh* InPlacingMesh, ECollisionChannel InCollisionChannel);

	UFUNCTION()
	void BeginBlueprintPlacement(ABlueprintActor* InPlacingActor, ECollisionChannel InCollisionChannel);

	UFUNCTION()
	bool EndBlueprintPlacement();

	UFUNCTION()
	void SpawnPlacedObject(UStaticMesh* InStaticMesh);

	UFUNCTION()
	void Rotate();

	UFUNCTION()
	void Scale(bool bIsUp);

	UPROPERTY()
	FConstructionObjectPlacedDelegate OnConstructionObjectPlacedHandle;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category=Constructor)
	TSubclassOf<AActor> PlaceableActorClass;
	
	UPROPERTY(EditAnywhere, Category=Constructor)
	UMaterialInterface* PlacementMaterial;
	
	UPROPERTY(EditAnywhere, Category=Constructor)
	UMaterialInterface* InvalidPlacementMaterial;

	UPROPERTY(EditAnywhere, Category=Workbench)
	int RotationSpeed;

	UPROPERTY(EditAnywhere, Category=Workbench)
	float ScaleStep;

	uint32 bIsInPlacementMode;
	ECollisionChannel PlacingGroundTraceChannel;

	UPROPERTY()
	AActor* PlaceableActor;

	UFUNCTION()
	void EndPlacement();
	
	UFUNCTION()
	void UpdatePlacement() const;

	UFUNCTION()
	void InitializeBlueprintForPlacement(const ABlueprintActor* InBlueprint) const;

};
