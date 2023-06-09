// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructorPlayerController.h"

#include "Actors/WorkbenchActor.h"
#include "Components/BlueprintManagerActorComponent.h"
#include "Components/ConstructionActorComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AConstructorPlayerController::AConstructorPlayerController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructionComp = CreateDefaultSubobject<UConstructionActorComponent>("Constructor");

	bIsPlacingBlueprint = false;
}

// Called when the game starts or when spawned
void AConstructorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InputComponent->BindAction(FName("Place"), IE_Pressed, this, &ThisClass::BeginBlueprintPlace);
	InputComponent->BindAction(FName("LeftClick"), IE_Pressed, this, &ThisClass::EndBlueprintPlace);

	InputComponent->BindAxis(FName("SwitchBlueprint"), this, &ThisClass::SwitchPlacingBlueprint);

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorkbenchActor::StaticClass(), OutActors);
	for (const auto& Actor : OutActors)
	{
		const auto Workbench = Cast<AWorkbenchActor>(Actor);
		Workbench->ActivationStatusChangedHandle.AddDynamic(this, &ThisClass::OnWorkbenchActivationStatusChanged);
	}
}

// Called every frame
void AConstructorPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AConstructorPlayerController::OnWorkbenchActivationStatusChanged(AWorkbenchActor* InWorkbench, bool bIsActive)
{
	if (bIsActive)
	{
		CurrentWorkbench = InWorkbench;
	}
	/*
	else
	{
		CurrentWorkbench = nullptr;
	}
*/
}

void AConstructorPlayerController::BeginBlueprintPlace()
{
	if (IsValid(CurrentWorkbench))
	{
		if (const auto BlueprintManager = CurrentWorkbench->GetBlueprintManager())
		{
			const auto Blueprints = BlueprintManager->GetBlueprints();
			if (Blueprints.Num() > 0)
			{
				const auto Blueprint = Blueprints[0];
				if (const auto NewBlueprintActor = BlueprintManager->GetBlueprint(Blueprint.BPName))
				{
					ConstructionComp->BeginBlueprintPlacement(NewBlueprintActor, ECC_GameTraceChannel1);
					bIsPlacingBlueprint = true;
				}
			}
		}
	}
}

void AConstructorPlayerController::EndBlueprintPlace()
{
	if (IsValid(ConstructionComp))
	{
		if (ConstructionComp->EndBlueprintPlacement())
		{
			bIsPlacingBlueprint = false;
		}
	}
}

void AConstructorPlayerController::SwitchPlacingBlueprint(float InAxisValue)
{
	if (InAxisValue == 0)
		return;

	if (!bIsPlacingBlueprint)
		return;

	if (IsValid(CurrentWorkbench))
	{
		if (const auto BlueprintManagerComp = CurrentWorkbench->GetBlueprintManager())
		{
			const auto Blueprints = BlueprintManagerComp->GetBlueprints();

			if (Blueprints.Num() > 0)
			{
				if (InAxisValue > 0)
				{
					CurrentBlueprintIndex++;
					if (CurrentBlueprintIndex == Blueprints.Num())
					{
						CurrentBlueprintIndex = 0;
					}
				}
				else
				{
					CurrentBlueprintIndex--;
					if (CurrentBlueprintIndex < 0)
					{
						CurrentBlueprintIndex = Blueprints.Num() - 1;
					}
				}

				const auto BPName = Blueprints[CurrentBlueprintIndex].BPName;
				const auto BlueprintActor = BlueprintManagerComp->GetBlueprint(BPName);

				ConstructionComp->SwitchPlacingBlueprint(BlueprintActor);
			}
		}
	}
}
