// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomGameView.h"

#include "Blueprint/WidgetTree.h"

UCustomGameView::UCustomGameView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UCustomGameView::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCustomGameView::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TSharedRef<SWidget> UCustomGameView::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

	if (RootWidget && WidgetTree)
	{
		this->CanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(TEXT("")));
		RootWidget->AddChild(this->CanvasPanel);

		this->OverlayWidget = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName(TEXT("")));
		CanvasPanel->AddChild(this->OverlayWidget);
	}

	return Widget;
}