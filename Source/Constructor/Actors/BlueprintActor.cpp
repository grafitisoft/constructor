// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintActor.h"

#include "Constructor/Constructor.h"
#include "Constructor/Components/PlaceableActorComponent.h"


// Sets default values
ABlueprintActor::ABlueprintActor()
{
	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = DefaultRootComponent;
}

void ABlueprintActor::AddBlueprintComponent(AActor* InComponentActor)
{
	if (!BlueprintComponents.Contains(InComponentActor))
	{
		this->BlueprintComponents.Add(InComponentActor);
	}
}

bool ABlueprintActor::IsPlacementValid()
{
	bool bIsValid = true;

	for(const auto ComponentActor : BlueprintComponents )
	{
		if (const auto PlaceableComponent = ComponentActor->FindComponentByClass<UPlaceableActorComponent>())
		{
			if (!PlaceableComponent->IsPlacementValid())
			{
				bIsValid = false;
				break;
			}
		}
	}
	
	return bIsValid;
}

void ABlueprintActor::Destroyed()
{
	for(const auto ComponentActor : BlueprintComponents)
	{
		ComponentActor->Destroy();
	}
	
	Super::Destroyed();
}
