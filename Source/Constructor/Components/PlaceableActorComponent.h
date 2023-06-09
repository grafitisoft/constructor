// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlaceableActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CONSTRUCTOR_API UPlaceableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlaceableActorComponent();

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

public:
	UFUNCTION()
	bool IsPlacementValid() const { return bIsPlacementValid; }
	
	UFUNCTION()
	void SetMaterials(UMaterialInterface* InValidMaterial, UMaterialInterface* InInvalidMaterial);
	
private:
	bool bIsPlacementValid;

	UPROPERTY()
	UMaterialInterface* PlacementMaterial;

	UPROPERTY()
	UMaterialInterface* InvalidPlacementMaterial;

	UPROPERTY()
	UMaterialInterface* InitialOwnerMaterial;

	UFUNCTION()
	void CheckValidPlacement();

	UFUNCTION()
	void OnOverlappedActor(AActor* InOverlappedActor, AActor* InOtherActor);
};
