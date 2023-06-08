// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkbenchActor.h"

#include "BlueprintActor.h"
#include "Components/BoxComponent.h"
#include "Components/Button.h"
#include "Components/WidgetComponent.h"
#include "Constructor/Constructor.h"
#include "Constructor/ConstructorCharacter.h"
#include "Constructor/Components/BlueprintManagerActorComponent.h"
#include "Constructor/Components/ConstructionActorComponent.h"
#include "Constructor/Support/BlueprintObject.h"
#include "Constructor/UI/WorkbenchWidget.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AWorkbenchActor::AWorkbenchActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BoxColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("User Interface"));
	ConstructorComponent = CreateDefaultSubobject<UConstructionActorComponent>(TEXT("Constructor"));
	BlueprintManagerComponent = CreateDefaultSubobject<UBlueprintManagerActorComponent>(TEXT("Blueprint Manager"));
	
	OriginComponent = CreateDefaultSubobject<UStaticMeshComponent>("Origin");
	OriginComponent->AttachToComponent(BoxColliderComponent,FAttachmentTransformRules::KeepRelativeTransform);
	
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
		}
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

void AWorkbenchActor::OnBtnSaveClicked()
{
	for (const auto Actor : PlacedConstructionActors)
	{
		const auto NewBlueprintObject = NewObject<UBlueprintObject>();

		if (auto MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>())
		{
			const auto StaticMesh = MeshComp->GetStaticMesh();
			NewBlueprintObject->MeshPath = StaticMesh->GetPathName();
		}
		
		auto RelativeLocation = Actor->GetActorLocation() - OriginComponent->GetComponentLocation();
		RelativeLocation.Z = 0;
		NewBlueprintObject->LocalPosition = RelativeLocation;

		NewBlueprintObject->Scale = Actor->GetActorScale3D();
		NewBlueprintObject->Rotation = Actor->GetActorRotation().Euler();
		
		BlueprintManagerComponent->AddBlueprintObject(NewBlueprintObject);

		Actor->Destroy();
	}

	PlacedConstructionActors.Empty();
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

	SelectConstructMesh();
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
	
	SelectConstructMesh();
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
		ConstructorComponent->Scale(InAxisValue > 0 );
	}
}


void AWorkbenchActor::OnConstructionObjectPlaced(UConstructionActorComponent* InConstructionComponent, AActor* InNewActor)
{
	/*
	if (IsValid(InNewActor))
	{
		if (IsValid(BlueprintManagerComponent))
		{
			BlueprintManagerComponent->AddBlueprintObject(InNewActor);
		}
	}
*/
	PlacedConstructionActors.Add(InNewActor);
}
