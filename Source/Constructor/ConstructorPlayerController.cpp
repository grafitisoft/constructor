// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructorPlayerController.h"

#include "Actors/WorkbenchActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AConstructorPlayerController::AConstructorPlayerController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AConstructorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor *> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorkbenchActor::StaticClass(), OutActors);
	for(const auto& Actor : OutActors )
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
	else
	{
		CurrentWorkbench = nullptr;
	}
}
