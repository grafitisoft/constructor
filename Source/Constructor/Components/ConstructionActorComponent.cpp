// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructionActorComponent.h"

#include "ClickableActorComponent.h"
#include "PlaceableActorComponent.h"


// Sets default values for this component's properties
UConstructionActorComponent::UConstructionActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UConstructionActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UConstructionActorComponent::UpdatePlacingConstructMesh(UStaticMesh* InNewMesh)
{
	if (bIsInPlacementMode && PlaceableActor)
	{
		if (const auto MeshComponent = PlaceableActor->FindComponentByClass<UStaticMeshComponent>())
		{
			MeshComponent->SetStaticMesh(InNewMesh);
		}
	}
}


void UConstructionActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsInPlacementMode)
	{
		UpdatePlacement();
	}
}

void UConstructionActorComponent::BeginActorPlacement(AActor* InPlacingActor, ECollisionChannel InCollisionChannel)
{
	if (bIsInPlacementMode)
		return;

	bIsInPlacementMode = true;
	
	PlacingGroundTraceChannel = InCollisionChannel;

	PlaceableActor = InPlacingActor;
}

void UConstructionActorComponent::BeginPlacement(UStaticMesh* InPlaceingMensh, ECollisionChannel InCollisionChannel)
{
	if (bIsInPlacementMode)
		return;

	bIsInPlacementMode = true;

	PlacingGroundTraceChannel = InCollisionChannel;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FVector Location = FVector(0, 0, -1000);
	const FRotator Rotation = FRotator::ZeroRotator;

	if (const auto TheWorld = GetWorld())
	{
		PlaceableActor = TheWorld->SpawnActor(PlaceableActorClass, &Location, &Rotation, SpawnParams);

		if (const auto MeshComp = PlaceableActor->FindComponentByClass<UStaticMeshComponent>())
		{
			MeshComp->SetStaticMesh(InPlaceingMensh);
		}
		
		const auto PlaceableActorComponent = Cast<UPlaceableActorComponent>(
			PlaceableActor->AddComponentByClass(UPlaceableActorComponent::StaticClass(), false, FTransform::Identity, false));
		PlaceableActorComponent->SetMaterials(PlacementMaterial, InvalidPlacementMaterial);
		
		if (const auto ClickableComponent = PlaceableActor->FindComponentByClass<UClickableActorComponent>())
		{
			ClickableComponent->DestroyComponent();
		}
	}
}

void UConstructionActorComponent::EndPlacement()
{
	if (!bIsInPlacementMode)
		return;

	bIsInPlacementMode = false;

	if (IsValid(PlaceableActor))
	{
		PlaceableActor->Destroy();
	}
}

void UConstructionActorComponent::UpdatePlacement() const
{
	if (const auto PlayerController = GetOwner()->GetWorld()->GetFirstPlayerController())
	{
		FVector WorldLocation;
		FVector WorldDirection;

		PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
		FVector End = WorldLocation + WorldDirection * 10000;

		FHitResult OutHit;
		GetWorld()->LineTraceSingleByChannel(OutHit, WorldLocation, End, PlacingGroundTraceChannel);

		if (OutHit.bBlockingHit)
		{
			PlaceableActor->SetActorLocation(OutHit.Location);
		}
	}
}

void UConstructionActorComponent::SpawnPlacedObject(UStaticMesh* InStaticMesh)
{
	if (bIsInPlacementMode && PlaceableActor)
	{
		if (const auto PlaceableComponent = Cast<UPlaceableActorComponent>(
			PlaceableActor->GetComponentByClass(UPlaceableActorComponent::StaticClass())))
		{
			if (PlaceableComponent->IsPlacementValid())
			{
				const auto Location = PlaceableActor->GetActorLocation();
				const auto Rotation = PlaceableActor->GetActorRotation();

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				const auto NewActor = GetWorld()->SpawnActor(PlaceableActorClass, &Location, &Rotation, SpawnParams);
				if (const auto MeshComp = NewActor->FindComponentByClass<UStaticMeshComponent>())
				{
					MeshComp->SetStaticMesh(InStaticMesh);
				}

				OnConstructionObjectPlacedHandle.Broadcast(this, NewActor);
				
				EndPlacement();
			}
		}
	}
}
