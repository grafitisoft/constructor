// Fill out your copyright notice in the Description page of Project Settings.


#include "ClickableActorComponent.h"

#include "Constructor/Constructor.h"


// Sets default values for this component's properties
UClickableActorComponent::UClickableActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bIsSelected = false;
}


// Called when the game starts
void UClickableActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetOwner()->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	GetOwner()->OnBeginCursorOver.AddDynamic(this, &ThisClass::OnCursorOver);
	GetOwner()->OnEndCursorOver.AddDynamic(this, &ThisClass::OnCursorExit);
}


// Called every frame
void UClickableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UClickableActorComponent::OnClicked(AActor* InActorClicked, FKey InButtonPressed)
{
	UE_LOG(LogConstructor, Log, TEXT("Clicked Actor:%s"), *InActorClicked->GetName());

	if (const auto OwnerActor = GetOwner())
	{
		if (const auto StaticMeshComponent = OwnerActor->FindComponentByClass<UStaticMeshComponent>())
		{
			bIsSelected = !bIsSelected;

			if (bIsSelected)
			{
				StaticMeshComponent->SetMaterial(0, SelectedMaterial);
			}
			else
			{
				StaticMeshComponent->SetMaterial(0, DeSelectedMaterial);
			}
		}
	}
}

void UClickableActorComponent::OnCursorOver(AActor* InActorCursorOver)
{
	if (const auto OwnerActor = GetOwner())
	{
		if (const auto StaticMeshComponent = OwnerActor->FindComponentByClass<UStaticMeshComponent>())
		{
			StaticMeshComponent->SetRenderCustomDepth(true);
		}
	}
}

void UClickableActorComponent::OnCursorExit(AActor* InActorCursorOver)
{
	if (const auto OwnerActor = GetOwner())
	{
		if (const auto StaticMeshComponent = OwnerActor->FindComponentByClass<UStaticMeshComponent>())
		{
			StaticMeshComponent->SetRenderCustomDepth(false);
		}
	}
}
