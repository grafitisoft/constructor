// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlueprintManagerActorComponent.generated.h"


class ABlueprintActor;
class UBlueprintObject;

/** Data wrapper to store in a TMap, since TMaps do not support nested collection, i.e TArray, as ValueType */
USTRUCT()
struct FBlueprintData
{
	GENERATED_BODY()

	UPROPERTY()
	FString BPName;

	UPROPERTY()
	TArray<UBlueprintObject *> Components;
};

/** Manager class to build Blueprint Actors given component actors
 * Workbench owns this manager to communicate. In exploration, player controller access this from the last visited Wokrkbench
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CONSTRUCTOR_API UBlueprintManagerActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBlueprintManagerActorComponent();

	UFUNCTION()
	void NewBlueprint(FString InName, TArray<AActor*> InComponents, const FVector& InOriginLocation);
	
	UFUNCTION()
	ABlueprintActor* GetBlueprint(const FString& InBPName) const;

	UFUNCTION()
	TArray<FBlueprintData> GetBlueprints() const;

private:
	UPROPERTY()
	TMap<FString, FBlueprintData> Blueprints;

	UPROPERTY()
	TSubclassOf<ABlueprintActor> BlueprintActorClass;
	
	UPROPERTY()
	TSubclassOf<AActor> BlueprintComponentActorClass;
};
