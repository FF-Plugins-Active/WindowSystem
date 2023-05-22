// Fill out your copyright notice in the Description page of Project Settings.

#include "WindowManager.h"
#include "WindowSystemBPLibrary.h"

// Sets default values
AWindowManager::AWindowManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWindowManager::BeginPlay()
{
	this->AddDragDropHandlerToMV();
	
	Super::BeginPlay();
}

// Called when the game ends or when destroyed
void AWindowManager::EndPlay(EEndPlayReason::Type Reason)
{
	this->CloseAllWindows();

	this->RemoveDragDropHandlerFromMV();
	
	Super::EndPlay(Reason);
}

// Called every frame
void AWindowManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWindowManager::NotifyWindowMoved(const TSharedRef<SWindow>& Window)
{
	AWindowManager::OnWindowMoved(Window.Get().GetTag());
}

void AWindowManager::NotifyWindowClosed(const TSharedRef<SWindow>& Window)
{
	AWindowManager::OnWindowClosed(Window.Get().GetTag());
}

void AWindowManager::AddDragDropHandlerToMV()
{
	DragDropHandler.OwnerActor = this;

	HWND WindowHandle = reinterpret_cast<HWND>(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());

	DragAcceptFiles(WindowHandle, true);

	FWindowsApplication* WindowApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();

	if (WindowApplication)
	{
		WindowApplication->AddMessageHandler(DragDropHandler);
	}
}

void AWindowManager::RemoveDragDropHandlerFromMV()
{
	FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();

	if (WindowsApplication)
	{
		WindowsApplication->RemoveMessageHandler(DragDropHandler);
	}
}

bool AWindowManager::CreateNewWindow(UWindowObject*& OutWindowObject, UPARAM(ref)UUserWidget*& InChildWidget, EWindowTypeBp In_Window_Type, bool bIsTopMost, bool bHasClose, bool bForceVolatile, bool bPreserveAspectRatio, bool bMinimized, bool bSupportsMaximized, bool bSupportsMinimized, bool bSetMirrorWindow, bool bShowOnTaskBar, bool bUseNativeBorder, FName InWindowTag, FText InWindowTitle, FText InToolTip, FLinearColor TitleColor, FVector2D WindowSize, FVector2D MinSize, FVector2D MaxSize, FVector2D WindowPosition, FMargin InBorder)
{
	// Styles
	EWindowType WindowType = EWindowType::GameWindow;
	switch (In_Window_Type)
	{
	case EWindowTypeBp::Normal:
		WindowType = EWindowType::Normal;
		break;
	case EWindowTypeBp::Menu:
		WindowType = EWindowType::Menu;
		break;
	case EWindowTypeBp::ToolTip:
		WindowType = EWindowType::ToolTip;
		break;
	case EWindowTypeBp::Notification:
		WindowType = EWindowType::Notification;
		break;
	case EWindowTypeBp::CursorDecorator:
		WindowType = EWindowType::CursorDecorator;
		break;
	case EWindowTypeBp::GameWindow:
		WindowType = EWindowType::GameWindow;
		break;
	}

	// Blueprints exposed UObject should contain TSharedPtr NOT TSharedRef.
	TSharedPtr<SWindow> WidgetWindow = SNew(SWindow)
		.bDragAnywhere(true)
		.ClientSize(WindowSize)
		.LayoutBorder(InBorder)
		.UserResizeBorder(InBorder)
		.Title(InWindowTitle)
		.ToolTipText(InToolTip)
		.ForceVolatile(bForceVolatile)
		.ShouldPreserveAspectRatio(bPreserveAspectRatio)
		.IsInitiallyMinimized(bMinimized)
		.FocusWhenFirstShown(true)
		.HasCloseButton(bHasClose)
		.SupportsMinimize(bSupportsMinimized)
		.SupportsMaximize(bSupportsMaximized)
		.SupportsTransparency(EWindowTransparency::PerWindow)
		.IsTopmostWindow(bIsTopMost)
		.Type(WindowType)
		.UseOSWindowBorder(bUseNativeBorder)
	;
	
	FWindowSizeLimits SizeLimits;
	SizeLimits.SetMinWidth(MinSize.X);
	SizeLimits.SetMinHeight(MinSize.Y);
	
	if (MaxSize.X == 0)
	{
		MaxSize.X = GEngine->GetGameUserSettings()->GetScreenResolution().X;
	}

	else
	{
		SizeLimits.SetMaxWidth(MaxSize.X);
	}

	if (MaxSize.Y == 0)
	{
		MaxSize.Y = GEngine->GetGameUserSettings()->GetScreenResolution().Y;
	}

	else
	{
		SizeLimits.SetMaxHeight(MaxSize.Y);
	}
	
	WidgetWindow->SetContent(InChildWidget->TakeWidget());
	WidgetWindow->SetAllowFastUpdate(true);
	WidgetWindow->SetMirrorWindow(bSetMirrorWindow);
	WidgetWindow->MoveWindowTo(WindowPosition);
	WidgetWindow->SetTag(InWindowTag);
	WidgetWindow->SetNativeWindowButtonsVisibility(bHasClose);
	WidgetWindow->SetForegroundColor(TitleColor);
	WidgetWindow->SetSizeLimits(SizeLimits);
	WidgetWindow->SetOnWindowMoved(FOnWindowClosed::CreateUObject(this, &AWindowManager::NotifyWindowMoved));
	WidgetWindow->SetOnWindowClosed(FOnWindowClosed::CreateUObject(this, &AWindowManager::NotifyWindowClosed));

	// Add created window to Slate.
	FSlateApplication::Get().AddWindow(WidgetWindow.ToSharedRef(), true);

	// Hide Window from Taskbar.
	HWND WidgetWindowHandle = reinterpret_cast<HWND>(WidgetWindow.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle());

	if (bShowOnTaskBar == true)
	{
		SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TRANSPARENT);
	}

	else
	{
		SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);
	}

	/*
	* Set window UObject parameters.
	* We need to crete UObject for moving SWindow, HWND, widget contents and other in blueprints.
	*/
	UWindowObject* WindowObject = NewObject<UWindowObject>();
	WindowObject->WindowPtr = WidgetWindow;
	WindowObject->ContentWidget = InChildWidget;
	WindowObject->WindowTag = InWindowTag;
	WindowObject->bShowOnTaskBar = bShowOnTaskBar;
	this->MAP_Windows.Add(InWindowTag, WindowObject);

	// Return values.
	OutWindowObject = WindowObject;

	return true;
}

bool AWindowManager::CloseWindow(UPARAM(ref)UWindowObject*& InWindowObject)
{
	if (IsValid(InWindowObject) == true)
	{
		if (IsValid(InWindowObject->ContentWidget) == true)
		{
			InWindowObject->ContentWidget->ReleaseSlateResources(true);
		}

		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			InWindowObject->WindowPtr->HideWindow();
			InWindowObject->WindowPtr->RequestDestroyWindow();
			InWindowObject->WindowPtr.Reset();
		}

		this->MAP_Windows.Remove(InWindowObject->WindowTag);
		InWindowObject = nullptr;
		return true;
	}

	else
	{
		return false;
	}
}

bool AWindowManager::CloseAllWindows()
{
	if (this->MAP_Windows.Num() > 0)
	{
		UPARAM(ref)TArray<UWindowObject*> ArrayWinObjects;
		this->MAP_Windows.GenerateValueArray(ArrayWinObjects);

		for (int32 WindowIndex = 0; WindowIndex < ArrayWinObjects.Num(); WindowIndex++)
		{
			this->CloseWindow(ArrayWinObjects[WindowIndex]);
		}

		return true;
	}

	else
	{
		return false;
	}
}

void AWindowManager::DetectHoveredWindow(FDelegateDetectHovered DelegateHovered)
{
	if (this->MAP_Windows.Num() <= 0)
	{
		DelegateHovered.ExecuteIfBound(false, nullptr);
		return;
	}
	
	UPARAM(ref)TArray<UWindowObject*> ArrayWinObjects;
	this->MAP_Windows.GenerateValueArray(ArrayWinObjects);

	for (int32 WindowIndex = 0; WindowIndex < ArrayWinObjects.Num(); WindowIndex++)
	{
		if (IsValid(ArrayWinObjects[WindowIndex]) == false)
		{
			DelegateHovered.ExecuteIfBound(false, nullptr);
			continue;
		}
		
		if (ArrayWinObjects[WindowIndex]->WindowPtr.IsValid() == false)
		{
			DelegateHovered.ExecuteIfBound(false, nullptr);
			continue;
		}
		
		if (ArrayWinObjects[WindowIndex]->WindowPtr.ToSharedRef().Get().IsHovered() == false)
		{
			DelegateHovered.ExecuteIfBound(false, nullptr);
			continue;
		}
		
		DelegateHovered.ExecuteIfBound(true, ArrayWinObjects[WindowIndex]);
		WindowIndex = 0;
		break;
	}

	return;
}

bool AWindowManager::SetFileDragDropSupport(UPARAM(ref)UWindowObject*& InWindowObject)
{
	if (IsValid(InWindowObject) == false)
	{
		return false;
	}
	
	if (InWindowObject->bIsFileDropEnabled == true)
	{
		return false;
	}

	InWindowObject->bIsFileDropEnabled = true;

	HWND WidgetWindowHandle = reinterpret_cast<HWND>(InWindowObject->WindowPtr.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle());
	DragAcceptFiles(WidgetWindowHandle, true);

	return true;
}