// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConstructionActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CONSTRUCTOR_API UConstructionActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UConstructionActorComponent();

	UFUNCTION()
	void UpdatePlacingConstructMesh(UStaticMesh* InNewMesh);
	
	UFUNCTION()
	void BeginPlacement(UStaticMesh* InPlaceingMensh, ECollisionChannel InCollisionChannel);

	UFUNCTION()
	void SpawnPlacedObject(UStaticMesh* InStaticMesh);
	
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

	uint32 bIsInPlacementMode;
	ECollisionChannel PlacingGroundTraceChannel;

	UPROPERTY()
	AActor* PlaceableActor;

	UFUNCTION()
	void EndPlacement();
	
	UFUNCTION()
	void UpdatePlacement() const;
};
