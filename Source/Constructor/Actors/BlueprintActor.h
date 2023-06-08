// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlueprintActor.generated.h"

UCLASS()
class CONSTRUCTOR_API ABlueprintActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABlueprintActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:
	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultRootComponent;
	
	UFUNCTION()
	void AddConstructorActor(AActor* InNewActor, const FVector& RelativeLocation);
};
