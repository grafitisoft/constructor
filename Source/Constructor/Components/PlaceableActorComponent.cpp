// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceableActorComponent.h"


// Sets default values for this component's properties
UPlaceableActorComponent::UPlaceableActorComponent()
{
	// ...
	bIsPlacementValid = true;
}


// Called when the game starts
void UPlaceableActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnOverlappedActor); 
	GetOwner()->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnOverlappedActor); 
}

void UPlaceableActorComponent::SetMaterials(UMaterialInterface* InValidMaterial, UMaterialInterface* InInvalidMaterial)
{
	PlacementMaterial = InValidMaterial;
	InvalidPlacementMaterial = InInvalidMaterial;

	CheckValidPlacement();
}

void UPlaceableActorComponent::CheckValidPlacement()
{
	if (const auto Owner = GetOwner())
	{
		TArray<AActor*> OutOverlappingActors;
		Owner->GetOverlappingActors(OutOverlappingActors, AActor::StaticClass());

		bIsPlacementValid = OutOverlappingActors.Num() == 0;

		TArray<UStaticMeshComponent*> OutMeshComponents;
		Owner->GetComponents<UStaticMeshComponent>(OutMeshComponents);
		for (const auto MeshComponent : OutMeshComponents)
		{
			if (bIsPlacementValid)
			{
				MeshComponent->SetMaterial(0, PlacementMaterial);
			}
			else
			{
				MeshComponent->SetMaterial(0, InvalidPlacementMaterial);
			}
		}
	}
}

void UPlaceableActorComponent::OnOverlappedActor(AActor* InOverlappedActor, AActor* InOtherActor)
{
	CheckValidPlacement();
}
