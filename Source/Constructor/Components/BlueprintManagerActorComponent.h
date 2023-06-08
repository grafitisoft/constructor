// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlueprintManagerActorComponent.generated.h"


class ABlueprintActor;
class UBlueprintObject;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CONSTRUCTOR_API UBlueprintManagerActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBlueprintManagerActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void AddBlueprintObject(UBlueprintObject* InBlueprintObject);

	UFUNCTION()
	AActor* GetBlueprint() const;

private:
	TArray<class UBlueprintObject*> Blueprint;

	UPROPERTY()
	TSubclassOf<ABlueprintActor> BlueprintActorClass;
	
	UFUNCTION()
	void ChildActorCreatedCallback(AActor* InActor);
	
	UPROPERTY()
	TSubclassOf<AActor> BlueprintComponentActorClass;
};
