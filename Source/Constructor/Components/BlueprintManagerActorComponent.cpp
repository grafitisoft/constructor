// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintManagerActorComponent.h"

#include "Constructor/Constructor.h"
#include "Constructor/Actors/BlueprintActor.h"
#include "Constructor/Support/BlueprintObject.h"


// Sets default values for this component's properties
UBlueprintManagerActorComponent::UBlueprintManagerActorComponent()
{
	static ConstructorHelpers::FClassFinder<AActor> ChildActorClassSearch(TEXT("/Game/Constructor/Blueprints/BP_ClickableActor"));
	if (ChildActorClassSearch.Succeeded())
	{
		BlueprintComponentActorClass = ChildActorClassSearch.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> BlueprintActorClassSearch(TEXT("/Game/Constructor/Blueprints/BP_BlueprintActor"));
	if (BlueprintActorClassSearch.Succeeded())
	{
		BlueprintActorClass = BlueprintActorClassSearch.Class;
	}
}


// Called when the game starts
void UBlueprintManagerActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UBlueprintManagerActorComponent::AddBlueprintObject(UBlueprintObject* InBlueprintObject)
{
	Blueprint.Add(InBlueprintObject);
}

AActor* UBlueprintManagerActorComponent::GetBlueprint() const
{
	if (Blueprint.Num() != 0 && BlueprintComponentActorClass)
	{
		const auto BlueprintActor = GetWorld()->SpawnActor(BlueprintActorClass);
		
		for (const auto& BlueprintObject : Blueprint)
		{
			const auto NewBlueprintComponentActor = GetWorld()->SpawnActor(BlueprintComponentActorClass);
			NewBlueprintComponentActor->SetActorRelativeLocation(BlueprintObject->LocalPosition);
			NewBlueprintComponentActor->SetActorRotation(FRotator::MakeFromEuler(BlueprintObject->Rotation)); // ????
			NewBlueprintComponentActor->SetActorScale3D(BlueprintObject->Scale);

			if (const auto MeshComp = NewBlueprintComponentActor->FindComponentByClass<UStaticMeshComponent>())
			{
				//if (const auto StaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule")))
				if (const auto StaticMesh = LoadObject<UStaticMesh>(nullptr, *BlueprintObject->MeshPath))
				{
					MeshComp->SetStaticMesh(StaticMesh);
				}
			}
			
			NewBlueprintComponentActor->AttachToActor(BlueprintActor, FAttachmentTransformRules::KeepRelativeTransform);
		}

		return BlueprintActor;
	}

	return nullptr;
}
