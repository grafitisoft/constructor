// Fill out your copyright notice in the Description page of Project Settings.


#include "ClickableActorComponent.h"

#include "Constructor/Constructor.h"


// Sets default values for this component's properties
UClickableActorComponent::UClickableActorComponent()
{
	bIsSelected = false;
}


// Called when the game starts
void UClickableActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const auto MeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>())
	{
		DeSelectedMaterial = MeshComponent->GetMaterial(0);
	}
	
	// ...
	GetOwner()->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	GetOwner()->OnBeginCursorOver.AddDynamic(this, &ThisClass::OnCursorOver);
	GetOwner()->OnEndCursorOver.AddDynamic(this, &ThisClass::OnCursorExit);
}

void UClickableActorComponent::OnClicked(AActor* InActorClicked, FKey InButtonPressed)
{
	if (const auto OwnerActor = GetOwner())
	{
		if (const auto StaticMeshComponent = OwnerActor->FindComponentByClass<UStaticMeshComponent>())
		{
			bIsSelected = !bIsSelected;

			SelectionStatusChangedHandle.Broadcast(GetOwner(), bIsSelected);
			
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
