// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructionActorComponent.h"

#include "ClickableActorComponent.h"
#include "PlaceableActorComponent.h"
#include "Constructor/Constructor.h"
#include "Constructor/Actors/BlueprintActor.h"


// Sets default values for this component's properties
UConstructionActorComponent::UConstructionActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	RotationSpeed = 5;
	ScaleStep = 0.05;
}

// Called when the game starts
void UConstructionActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UConstructionActorComponent::SwitchPlacingConstructMesh(UStaticMesh* InNewMesh) const
{
	if (bIsInPlacementMode && PlaceableActor)
	{
		if (const auto MeshComponent = PlaceableActor->FindComponentByClass<UStaticMeshComponent>())
		{
			MeshComponent->SetStaticMesh(InNewMesh);
		}
	}
}

void UConstructionActorComponent::SwitchPlacingBlueprint(ABlueprintActor* InNewActor)
{
	if (bIsInPlacementMode && PlaceableActor)
	{
		InitializeBlueprintForPlacement(InNewActor);

		PlaceableActor->Destroy();
		PlaceableActor = Cast<AActor>(InNewActor);
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

void UConstructionActorComponent::BeginBlueprintPlacement(ABlueprintActor* InPlacingActor, ECollisionChannel InCollisionChannel)
{
	if (bIsInPlacementMode)
		return;

	bIsInPlacementMode = true;

	PlacingGroundTraceChannel = InCollisionChannel;
	
	InitializeBlueprintForPlacement(InPlacingActor);
	
	PlaceableActor = Cast<AActor>(InPlacingActor);
}

void UConstructionActorComponent::BeginPlacement(UStaticMesh* InPlacingMesh, ECollisionChannel InCollisionChannel)
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
			MeshComp->SetStaticMesh(InPlacingMesh);
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

bool UConstructionActorComponent::EndBlueprintPlacement()
{
	if (!bIsInPlacementMode)
		return false;

	if (IsValid(PlaceableActor))
	{
		if (const auto Blueprint = Cast<ABlueprintActor>(PlaceableActor))
		{
			if (Blueprint->IsPlacementValid())
			{
				const auto Components = Blueprint->GetComponentActors();
				for(const auto ComponentActor : Components)
				{
					if (const auto PlaceableComp = ComponentActor->FindComponentByClass<UPlaceableActorComponent>())
					{
						PlaceableComp->DestroyComponent();
					}
				}
				
				bIsInPlacementMode = false;
			}
		}
	}

	return !bIsInPlacementMode;
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
	if (!IsValid(PlaceableActor))
		return;

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

void UConstructionActorComponent::InitializeBlueprintForPlacement(const ABlueprintActor* InBlueprint) const
{
	auto BlueprintComponents = InBlueprint->GetComponentActors();
	for (const auto ComponentActor : BlueprintComponents)
	{
		/** Add placeable actor component for validation check */
		const auto PlaceableActorComponent = Cast<UPlaceableActorComponent>(
			ComponentActor->AddComponentByClass(UPlaceableActorComponent::StaticClass(), false, FTransform::Identity, false));

		PlaceableActorComponent->SetMaterials(PlacementMaterial, InvalidPlacementMaterial);

		if (const auto ClickableComponent = ComponentActor->FindComponentByClass<UClickableActorComponent>())
		{
			/** Remove selection if exist */
			ClickableComponent->DestroyComponent();
		}		
	}
}

void UConstructionActorComponent::Rotate()
{
	if (bIsInPlacementMode && PlaceableActor)
	{
		const FRotator Rotation = PlaceableActor->GetActorRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		YawRotation.Yaw += (RotationSpeed);
		YawRotation.Yaw = FMath::ClampAngle(YawRotation.Yaw, 0, 359);

		PlaceableActor->SetActorRotation(YawRotation);
	}
}

void UConstructionActorComponent::Scale(bool bIsUp)
{
	if (bIsInPlacementMode && PlaceableActor)
	{
		auto CurrentScale = PlaceableActor->GetActorScale3D();

		if (bIsUp)
		{
			CurrentScale = FVector(CurrentScale.X + ScaleStep, CurrentScale.Y + ScaleStep, CurrentScale.Z + ScaleStep);
		}
		else
		{
			CurrentScale = FVector(CurrentScale.X - ScaleStep, CurrentScale.Y - ScaleStep, CurrentScale.Z - ScaleStep);
		}

		CurrentScale = ClampVector(CurrentScale, FVector(0.5, 0.5, 0.5), FVector(3, 3, 3));

		PlaceableActor->SetActorScale3D(CurrentScale);
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
				const auto Scale = PlaceableActor->GetActorScale3D();

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				const auto NewActor = GetWorld()->SpawnActor(PlaceableActorClass, &Location, &Rotation, SpawnParams);
				NewActor->SetActorScale3D(Scale);

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
