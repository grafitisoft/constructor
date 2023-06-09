// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkbenchActor.h"

#include "BlueprintActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Constructor/Constructor.h"
#include "Constructor/ConstructorCharacter.h"
#include "Constructor/Components/BlueprintManagerActorComponent.h"
#include "Constructor/Components/ClickableActorComponent.h"
#include "Constructor/Components/ConstructionActorComponent.h"
#include "Constructor/Support/BlueprintObject.h"
#include "Constructor/UI/ConfirmDialgueWidget.h"
#include "Constructor/UI/WorkbenchWidget.h"
#include "Kismet/GameplayStatics.h"

static int NameIndex = 1;

// Sets default values
AWorkbenchActor::AWorkbenchActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BoxColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("User Interface"));
	ConstructorComponent = CreateDefaultSubobject<UConstructionActorComponent>(TEXT("Constructor"));
	BlueprintManagerComponent = CreateDefaultSubobject<UBlueprintManagerActorComponent>(TEXT("Blueprint Manager"));

	OriginComponent = CreateDefaultSubobject<UStaticMeshComponent>("Origin");
	OriginComponent->AttachToComponent(BoxColliderComponent, FAttachmentTransformRules::KeepRelativeTransform);

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
			WorkbenchWidget->BtnSaveBlueprint->OnClicked.AddDynamic(this, &ThisClass::OnBtnSaveClicked);
			WorkbenchWidget->BtnDeleteSelected->OnClicked.AddDynamic(this, &ThisClass::OnBtnDeleteSelectedClicked);
		}
	}

	ConfirmationDialogueWidget = CreateWidget<UConfirmDialgueWidget>(WorkbenchWidget, DialogueWidgetClass, FName("Confirmation"));
	if (ConfirmationDialogueWidget)
	{
		ConfirmationDialogueWidget->ConfirmationDialogueClosedHandle.AddDynamic(this, &ThisClass::OnConfirmationDialogueClosed);
		ConfirmationDialogueWidget->AddToViewport();
		ConfirmationDialogueWidget->SetVisibility(ESlateVisibility::Collapsed);
	}


	ConstructorComponent->OnConstructionObjectPlacedHandle.AddDynamic(this, &ThisClass::OnConstructionObjectPlaced);
	BoxColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapped);
	//BoxColliderComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnded);

	if (const auto PlayerController = GetWorld()->GetFirstPlayerController())
	{
		EnableInput(PlayerController);

		InputComponent->BindAction(FName("LeftClick"), IE_Pressed, this, &ThisClass::OnLeftMouseClicked);
		InputComponent->BindAction(FName("NextPiece"), IE_Pressed, this, &ThisClass::OnSelectNextPiece);
		InputComponent->BindAction(FName("PrevPiece"), IE_Pressed, this, &ThisClass::OnSelectPreviousPiece);

		InputComponent->BindAxis(FName("RotatePiece"), this, &ThisClass::OnRotate);
		InputComponent->BindAxis(FName("ScalePiece"), this, &ThisClass::OnScale);
	}

	SwitchConstructMesh();
}

void AWorkbenchActor::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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

void AWorkbenchActor::ToggleUI()
{
	if (WorkbenchWidget)
	{
		if (bIsPlayerIn)
		{
			WorkbenchWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			EnableInput(GetWorld()->GetFirstPlayerController());
		}
		else
		{
			WorkbenchWidget->SetVisibility(ESlateVisibility::Hidden);

			DisableInput(GetWorld()->GetFirstPlayerController());
		}
	}
}

void AWorkbenchActor::SwitchConstructMesh()
{
	if (ConstructionParts.Num() > 0)
	{
		CurrentConstructMesh = ConstructionParts[CurrentSelectedMeshIndex];

		if (IsValid(ConstructorComponent))
		{
			ConstructorComponent->SwitchPlacingConstructMesh(CurrentConstructMesh);
		}
	}
}

void AWorkbenchActor::OnBtnPlaceClicked()
{
	if (IsValid(ConstructorComponent))
	{
		ConstructorComponent->BeginPlacement(CurrentConstructMesh, ECC_GameTraceChannel2); // Workbench channel
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}
}

void AWorkbenchActor::OnBtnSaveClicked()
{
	if (IsValid(BlueprintManagerComponent))
	{
		const FString BPName = FString::Printf(TEXT("Blueprint-%d"), NameIndex);
		BlueprintManagerComponent->NewBlueprint(BPName,
		                                        PlacedConstructionActors,
		                                        OriginComponent->GetComponentLocation());
		++NameIndex;
	}

	PlacedConstructionActors.Empty();
	SelectedConstructionActors.Empty();
}

void AWorkbenchActor::OnBtnDeleteSelectedClicked()
{
	if (SelectedConstructionActors.Num() != 0)
	{
		if (ConfirmationDialogueWidget)
		{
			ConfirmationDialogueWidget->ConfirmationText->SetText(FText::FromString("Selected components will be removed from blueprint?"));
			ConfirmationDialogueWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AWorkbenchActor::OnLeftMouseClicked()
{
	if (IsValid(ConstructorComponent) && IsValid(CurrentConstructMesh))
	{
		ConstructorComponent->SpawnPlacedObject(CurrentConstructMesh);
	}
}

void AWorkbenchActor::OnSelectNextPiece()
{
	if (!bIsPlayerIn)
		return;

	CurrentSelectedMeshIndex++;
	if (CurrentSelectedMeshIndex == ConstructionParts.Num())
	{
		CurrentSelectedMeshIndex = 0;
	}

	SwitchConstructMesh();
}

void AWorkbenchActor::OnSelectPreviousPiece()
{
	if (!bIsPlayerIn)
		return;

	CurrentSelectedMeshIndex--;
	if (CurrentSelectedMeshIndex < 0)
	{
		CurrentSelectedMeshIndex = ConstructionParts.Num() - 1;
	}

	SwitchConstructMesh();
}

void AWorkbenchActor::OnRotate(float InAxisValue)
{
	if (InAxisValue == 0)
		return;

	if (IsValid(ConstructorComponent))
	{
		ConstructorComponent->Rotate();
	}
}

void AWorkbenchActor::OnScale(float InAxisValue)
{
	if (InAxisValue == 0)
		return;

	if (IsValid(ConstructorComponent))
	{
		ConstructorComponent->Scale(InAxisValue > 0);
	}
}


void AWorkbenchActor::OnConstructionObjectPlaced(UConstructionActorComponent* InConstructionComponent, AActor* InNewActor)
{
	PlacedConstructionActors.Add(InNewActor);

	if (const auto ClickableComponent = InNewActor->FindComponentByClass<UClickableActorComponent>())
	{
		ClickableComponent->SelectionStatusChangedHandle.AddDynamic(this, &ThisClass::OnConstructionActorStatusChanged);
	}
}

void AWorkbenchActor::OnConstructionActorStatusChanged(AActor* InActor, bool IsSelected)
{
	if (IsValid(InActor))
	{
		if (IsSelected && !SelectedConstructionActors.Contains(InActor))
		{
			SelectedConstructionActors.Add(InActor);
		}
		else if (SelectedConstructionActors.Contains(InActor))
		{
			SelectedConstructionActors.Remove(InActor);
		}
	}
}

void AWorkbenchActor::OnConfirmationDialogueClosed(bool IsConfirmed)
{
	if (IsConfirmed)
	{
		for (auto Actor : SelectedConstructionActors)
		{
			PlacedConstructionActors.Remove(Actor);
			Actor->Destroy();
		}

		SelectedConstructionActors.Empty();
	}
}
