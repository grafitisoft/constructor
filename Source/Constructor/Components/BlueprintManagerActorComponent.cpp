// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintManagerActorComponent.h"

#include "Constructor/Constructor.h"
#include "Constructor/Actors/BlueprintActor.h"
#include "Constructor/Support/BlueprintObject.h"


// Sets default values for this component's properties
UBlueprintManagerActorComponent::UBlueprintManagerActorComponent()
{
	static ConstructorHelpers::FClassFinder<AActor> BlueprintComponentClassSearch(TEXT("/Game/Constructor/Blueprints/BP_ClickableActor"));
	if (BlueprintComponentClassSearch.Succeeded())
	{
		BlueprintComponentActorClass = BlueprintComponentClassSearch.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> BlueprintActorClassSearch(TEXT("/Game/Constructor/Blueprints/BP_BlueprintActor"));
	if (BlueprintActorClassSearch.Succeeded())
	{
		BlueprintActorClass = BlueprintActorClassSearch.Class;
	}
}

void UBlueprintManagerActorComponent::NewBlueprint(FString InName, TArray<AActor*> InComponents, const FVector& OriginLocation)
{
	TArray<class UBlueprintObject*> Components;

	for (const auto Actor : InComponents)
	{
		const auto NewBlueprintObject = NewObject<UBlueprintObject>();

		if (const auto MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>())
		{
			const auto StaticMesh = MeshComp->GetStaticMesh();
			NewBlueprintObject->MeshPath = StaticMesh->GetPathName();
		}

		auto RelativeLocation = Actor->GetActorLocation() - OriginLocation;
		RelativeLocation.Z = 0;
		NewBlueprintObject->LocalPosition = RelativeLocation;

		NewBlueprintObject->Scale = Actor->GetActorScale3D();
		NewBlueprintObject->Rotation = Actor->GetActorRotation().Euler();

		Components.Add(NewBlueprintObject);

		Actor->Destroy();
	}

	FBlueprintData BP;
	BP.BPName = FString(InName);
	BP.Components = Components;

	if (!Blueprints.Contains(InName))
	{
		Blueprints.Add(InName, BP);
	}
}

ABlueprintActor* UBlueprintManagerActorComponent::GetBlueprint(const FString& InBPName) const
{
	if (Blueprints.Contains(InBPName))
	{
		const auto BP = Blueprints[InBPName];

		if (BP.Components.Num() != 0 && BlueprintComponentActorClass)
		{
			const auto BlueprintActor = GetWorld()->SpawnActor<ABlueprintActor>(BlueprintActorClass);

			for (const auto& BlueprintObject : BP.Components)
			{
				if (IsValid(BlueprintObject))
				{
					const auto NewBlueprintComponentActor = GetWorld()->SpawnActor(BlueprintComponentActorClass);

					NewBlueprintComponentActor->SetActorRelativeLocation(BlueprintObject->LocalPosition);
					NewBlueprintComponentActor->SetActorRotation(FRotator::MakeFromEuler(BlueprintObject->Rotation)); // ????
					NewBlueprintComponentActor->SetActorScale3D(BlueprintObject->Scale);

					if (const auto MeshComp = NewBlueprintComponentActor->FindComponentByClass<UStaticMeshComponent>())
					{
						if (const auto StaticMesh = LoadObject<UStaticMesh>(nullptr, *BlueprintObject->MeshPath))
						{
							MeshComp->SetStaticMesh(StaticMesh);
						}
					}

					NewBlueprintComponentActor->AttachToActor(BlueprintActor, FAttachmentTransformRules::KeepRelativeTransform);
					BlueprintActor->AddBlueprintComponent(NewBlueprintComponentActor);
				}
			}

			return BlueprintActor;
		}
	}

	return nullptr;
}

TArray<FBlueprintData> UBlueprintManagerActorComponent::GetBlueprints() const
{
	TArray<FBlueprintData> OutBlueprints;

	Blueprints.GenerateValueArray(OutBlueprints);

	return OutBlueprints;
	
}
