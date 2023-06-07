// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkbenchActor.h"

#include "Components/BoxComponent.h"
#include "Components/Button.h"
#include "Components/WidgetComponent.h"
#include "Constructor/Constructor.h"
#include "Constructor/ConstructorCharacter.h"
#include "Constructor/Components/ConstructionActorComponent.h"
#include "Constructor/UI/WorkbenchWidget.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AWorkbenchActor::AWorkbenchActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BoxColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("User Interface"));
	ConstructorComponent = CreateDefaultSubobject<UConstructionActorComponent>(TEXT("Constructor"));
	
	bIsPlayerIn = false;
	CurrentSelectedMeshIndex = 0;
}

// Called when the game starts or when spawned
void AWorkbenchActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(WidgetComponent))
	{
		WorkbenchWidget = Cast<UWorkbenchWidget>(WidgetComponent->GetUserWidgetObject());
		if (WorkbenchWidget)
		{
			WorkbenchWidget->AddToViewport();
			WorkbenchWidget->SetVisibility(ESlateVisibility::Hidden);
			WorkbenchWidget->BtnPlace->OnClicked.AddDynamic(this, &ThisClass::OnBtnPlaceClicked);
		}
	}

	BoxColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapped);
	//BoxColliderComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnded);

	if (const auto PlayerController = GetWorld()->GetFirstPlayerController())
	{
		EnableInput(PlayerController);

		InputComponent->BindAction(FName("LeftClick"), IE_Pressed, this, &ThisClass::OnLeftMouseClicked);
		InputComponent->BindAxis(FName("SwitchPrevNextMesh"), this, &ThisClass::OnMouseWheelScrolled);
	}

	SelectConstructMesh();
}

void AWorkbenchActor::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogConstructor, Log, TEXT("Overlapped Actor:%s"), *OtherActor->GetName());

	if (Cast<AConstructorCharacter>(OtherActor))
	{
		bIsPlayerIn = !bIsPlayerIn;
		ToggleUI();

		ActivationStatusChangedHandle.Broadcast(this, bIsPlayerIn);
	}
}

void AWorkbenchActor::OnOverlapEnded(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     int32 OtherBodyIndex)
{
	UE_LOG(LogConstructor, Log, TEXT("Overlapped End Actor:%s"), *OtherActor->GetName());
	if (Cast<AConstructorCharacter>(OtherActor))
	{
		bIsPlayerIn = false;
		ToggleUI();
	}
}

void AWorkbenchActor::ToggleUI() const
{
	if (WorkbenchWidget)
	{
		if (bIsPlayerIn)
		{
			WorkbenchWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			const auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			PlayerController->SetInputMode(FInputModeGameAndUI());
		}
		else
		{
			WorkbenchWidget->SetVisibility(ESlateVisibility::Hidden);
			const auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			PlayerController->SetInputMode(FInputModeGameOnly());
		}
	}
}

void AWorkbenchActor::SelectConstructMesh()
{
	if (ConstructionParts.Num() > 0)
	{
		CurrentConstructMesh = ConstructionParts[CurrentSelectedMeshIndex];

		if (IsValid(ConstructorComponent))
		{
			ConstructorComponent->UpdatePlacingConstructMesh(CurrentConstructMesh);
		}
	}
}

void AWorkbenchActor::OnBtnPlaceClicked()
{
	UE_LOG(LogConstructor, Log, TEXT("Place Button Clicked"));

	if (IsValid(ConstructorComponent))
	{
		ConstructorComponent->BeginPlacement(CurrentConstructMesh,ECC_GameTraceChannel2); // Workbench channel	
	}
}

void AWorkbenchActor::OnLeftMouseClicked()
{
	if (IsValid(ConstructorComponent) && IsValid(CurrentConstructMesh))
	{
		ConstructorComponent->SpawnPlacedObject(CurrentConstructMesh);
	}
}

void AWorkbenchActor::OnMouseWheelScrolled(float InAxisValue)
{
	if (!bIsPlayerIn)
		return;

	if (InAxisValue == 0)
		return;
	
	if (InAxisValue > 0)
	{
		CurrentSelectedMeshIndex++;
		if (CurrentSelectedMeshIndex == ConstructionParts.Num())
		{
			CurrentSelectedMeshIndex = 0;
		}
	}
	else
	{
		CurrentSelectedMeshIndex--;
		if (CurrentSelectedMeshIndex < 0)
		{
			CurrentSelectedMeshIndex = ConstructionParts.Num() - 1;
		}
	}
	
	SelectConstructMesh();
}
