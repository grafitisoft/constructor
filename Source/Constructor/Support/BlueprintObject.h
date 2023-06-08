// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintObject.generated.h"

/**
 * 
 */
UCLASS()
class CONSTRUCTOR_API UBlueprintObject : public UObject
{
	GENERATED_BODY()

public:	
	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString MeshPath;

	UPROPERTY()
	FVector LocalPosition;

	UPROPERTY()
	FVector Scale;

	UPROPERTY()
	FVector Rotation;
};
