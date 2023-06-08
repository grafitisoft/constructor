// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintActor.h"

#include "Constructor/Constructor.h"


// Sets default values
ABlueprintActor::ABlueprintActor()
{
	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = DefaultRootComponent;
	
}

// Called when the game starts or when spawned
void ABlueprintActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABlueprintActor::AddConstructorActor(AActor* InNewActor, const FVector& RelativeLocation)
{
	if (IsValid(InNewActor))
	{
		if (const auto NewConstructorMeshComp = InNewActor->FindComponentByClass<UStaticMeshComponent>())
		{
			const auto NewActorComp = AddComponentByClass(UStaticMeshComponent::StaticClass(), false, InNewActor->GetTransform(), false);

			Cast<UStaticMeshComponent>(NewActorComp)->SetStaticMesh(NewConstructorMeshComp->GetStaticMesh());
			Cast<UStaticMeshComponent>(NewActorComp)->SetRelativeLocation(RelativeLocation);
		}
	}
}
