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
			NewBlueprintComponentActor->AttachToActor(BlueprintActor, FAttachmentTransformRules::KeepRelativeTransform);
			
			/*
			const auto ChildActorComp = NewObject<UChildActorComponent>(BlueprintActor);
			
			ChildActorComp->bEditableWhenInherited = true;
			ChildActorComp->SetMobility(EComponentMobility::Movable);
			ChildActorComp->RegisterComponent();
			ChildActorComp->SetChildActorClass(BlueprintComponentActorClass);

			const TFunction<void(AActor*)> CustomizerFunc = [BlueprintObject](AActor* InActor)
			{
				UE_LOG(LogConstructor, Log, TEXT("Relative location:%s"), *BlueprintObject->LocalPosition.ToString());
				InActor->SetActorRelativeLocation(BlueprintObject->LocalPosition);
			};

			ChildActorComp->CreateChildActor(CustomizerFunc);
			*/
			
		}

		return BlueprintActor;
	}

	return nullptr;
}

void UBlueprintManagerActorComponent::ChildActorCreatedCallback(AActor* InActor)
{
	
}
