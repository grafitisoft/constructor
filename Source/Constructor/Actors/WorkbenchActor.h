// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorkbenchActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWorkbenchActivationStatusChangedDelegate, AWorkbenchActor *, InWorkbench, bool, IsActive);

UCLASS()
class CONSTRUCTOR_API AWorkbenchActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorkbenchActor(const FObjectInitializer& ObjectInitializer);
	
	FWorkbenchActivationStatusChangedDelegate ActivationStatusChangedHandle;
	
	UFUNCTION()
	UStaticMesh* GetCurrentConstructMesh() const { return CurrentConstructMesh; }

	UFUNCTION()
	class UBlueprintManagerActorComponent* GetBlueprintManager() const { return BlueprintManagerComponent; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category=Workbench)
	class UBoxComponent* BoxColliderComponent;

	UPROPERTY(EditAnywhere, Category=Workbench)
	class UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, Category=Workbench)
	class UConstructionActorComponent* ConstructorComponent;

	UPROPERTY(EditAnywhere, Category=Workbench)
	class UBlueprintManagerActorComponent * BlueprintManagerComponent;

	UPROPERTY(EditAnywhere, Category=Workbench)
	TArray<UStaticMesh*> ConstructionParts;


	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent,
	                  AActor* OtherActor,
	                  UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex,
	                  bool bFromSweep,
	                  const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnded(UPrimitiveComponent* OverlappedComponent,
	                    AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere, Category=Workbench)
	UStaticMeshComponent* OriginComponent;
	
	UPROPERTY()
	class UWorkbenchWidget* WorkbenchWidget;

	int CurrentSelectedMeshIndex;
	uint32 bIsPlayerIn:1;

	void ToggleUI() const;
	void SelectConstructMesh();

	UPROPERTY()
	class ABlueprintActor* CurrentBlueprintActor;
	
	UPROPERTY()
	TArray<AActor *> PlacedConstructionActors;

	UPROPERTY()
	UStaticMesh* CurrentConstructMesh;

	UFUNCTION()
	void OnBtnPlaceClicked();

	UFUNCTION()
	void OnBtnSaveClicked();

	UFUNCTION()
	void OnLeftMouseClicked();
	
	UFUNCTION()
	void OnMouseWheelScrolled(float InAxisValue);

	UFUNCTION()
	void OnConstructionObjectPlaced(class UConstructionActorComponent* InConstructionComponent, AActor* InNewActor);
};
