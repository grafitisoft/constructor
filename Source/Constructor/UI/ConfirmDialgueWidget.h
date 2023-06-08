// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmDialgueWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConfirmationDialogueClosedDelegate, bool, IsConfirmed);

/**
 * 
 */
UCLASS()
class CONSTRUCTOR_API UConfirmDialgueWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ConfirmationText;

	UPROPERTY(meta=(BindWidget))
	class UButton* BtnOkay;

	UPROPERTY(meta=(BindWidget))
	UButton* BtnCancel;

	UPROPERTY()
	FConfirmationDialogueClosedDelegate ConfirmationDialogueClosedHandle;

private:

	UFUNCTION()
	void OnBtnOkayClicked();

	UFUNCTION()
	void OnBtnCancelClicked();
};
