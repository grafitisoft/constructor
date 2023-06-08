// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfirmDialgueWidget.h"

#include "Components/Button.h"

void UConfirmDialgueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnOkay->OnClicked.AddDynamic(this, &ThisClass::OnBtnOkayClicked);	
	BtnCancel->OnClicked.AddDynamic(this, &ThisClass::OnBtnCancelClicked);	

	SetVisibility(ESlateVisibility::Collapsed);
}

void UConfirmDialgueWidget::OnBtnOkayClicked()
{
	ConfirmationDialogueClosedHandle.Broadcast(true);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UConfirmDialgueWidget::OnBtnCancelClicked()
{
	ConfirmationDialogueClosedHandle.Broadcast(false);
	SetVisibility(ESlateVisibility::Collapsed);
}
