// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlueprintActor.generated.h"

/** Container class for components.
 * Components added via AddToActor in runtime, so we must manually delete child actors on Destroyed override */
UCLASS()
class CONSTRUCTOR_API ABlueprintActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABlueprintActor();

	UFUNCTION()
	void AddBlueprintComponent(AActor *InComponentActor);

	/** Check all child PlaceableActorComponents for validation */
	UFUNCTION()
	bool IsPlacementValid();
	
	UFUNCTION()
	FORCEINLINE TArray<AActor *> GetComponentActors() const { return BlueprintComponents; }
	
protected:
	virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultRootComponent;

	UPROPERTY()
	TArray<AActor *> BlueprintComponents;
};
