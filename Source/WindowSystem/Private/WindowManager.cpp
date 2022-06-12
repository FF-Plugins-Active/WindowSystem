// Fill out your copyright notice in the Description page of Project Settings.

#include "WindowManager.h"

// UE Includes.
#include "HAL/UnrealMemory.h"

// Sets default values
AWindowManager::AWindowManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWindowManager::BeginPlay()
{
	Super::BeginPlay();
	
	MW_DragDropHandler.OwnerActor = this;
	
	HWND Window = reinterpret_cast<HWND>(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());
	DragAcceptFiles(Window, true);
	
	FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
	WindowsApplication->AddMessageHandler(MW_DragDropHandler);
}

// Called when the game ends or when destroyed
void AWindowManager::EndPlay(EEndPlayReason::Type Reason)
{
	FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
	WindowsApplication->RemoveMessageHandler(MW_DragDropHandler);

	this->CloseAllWindows();
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

bool AWindowManager::CreateNewWindow(UPARAM(ref)UUserWidget*& InChildWidget, bool bIsTopMost, bool bHasClose, bool bForceVolatile, bool bPreserveAspectRatio, bool bMinimized, bool bSupportsMaximized, bool bSupportsMinimized, bool bSetMirrorWindow, bool bAllowFileDrop, FName InWindowTag, FText InWindowTitle, FText InToolTip, FVector2D WindowSize, FVector2D MinSize, FVector2D WindowPosition, FMargin InBorder, float InOpacity, FColor DropColor, UWindowObject*& OutWindowObject)
{
	// We need to crete UObject for moving SWindow, HWND, widget contents and other in blueprints.
	UWindowObject* WindowObject = NewObject<UWindowObject>();

	// Blueprints exposed UObject should contain TSharedPtr NOT TSharedRef.
	TSharedPtr<SWindow> WidgetWindow;

	WidgetWindow = SNew(SWindow)
		.bDragAnywhere(true)
		.ClientSize(WindowSize)
		.MinHeight(MinSize.Y)
		.MinWidth(MinSize.X)
		.LayoutBorder(InBorder)
		.UserResizeBorder(InBorder)
		.Title(InWindowTitle)
		.ToolTipText(InToolTip)
		.ForceVolatile(bForceVolatile)
		.ShouldPreserveAspectRatio(bPreserveAspectRatio)
		.IsInitiallyMinimized(bMinimized)
		.InitialOpacity(InOpacity)
		.FocusWhenFirstShown(true)
		.HasCloseButton(bHasClose)
		.SupportsMinimize(bSupportsMinimized)
		.SupportsMaximize(bSupportsMaximized)
		.SupportsTransparency(EWindowTransparency::PerWindow)
		.IsTopmostWindow(bIsTopMost)
		.Type(EWindowType::GameWindow)
		;
	
	WidgetWindow->SetContent(InChildWidget->TakeWidget());
	WidgetWindow->SetAllowFastUpdate(true);
	WidgetWindow->SetMirrorWindow(bSetMirrorWindow);
	WidgetWindow->MoveWindowTo(WindowPosition);
	WidgetWindow->SetTag(InWindowTag);

	FSlateApplication::Get().AddWindow(WidgetWindow.ToSharedRef(), true);

	HWND WidgetWindowHandle = reinterpret_cast<HWND>(WidgetWindow.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle());

	// Hide Window from Taskbar.
	long TaskbarHide = WS_EX_NOACTIVATE;
	SetWindowLong(WidgetWindowHandle, GWL_EXSTYLE, TaskbarHide);

	// Start File Drop Section.
	if (bAllowFileDrop == true)
	{
		DragAcceptFiles(WidgetWindowHandle, true);

		WindowObject->DragDropHandler.OwnerActor = this;

		FWindowsApplication* WindowsApplication = reinterpret_cast<FWindowsApplication*>(WidgetWindow.Get());
		WindowsApplication->AddMessageHandler(WindowObject->DragDropHandler);
	}

	WindowObject->bIsFileDropEnabled = bAllowFileDrop;
	WindowObject->WindowPtr = WidgetWindow;
	WindowObject->ContentWidget = InChildWidget;
	WindowObject->WindowTag = InWindowTag;

	// Record window BP object to window manager.
	this->MAP_Windows.Add(InWindowTag, WindowObject);

	// Initialize window events.
	WidgetWindow->SetOnWindowMoved(FOnWindowClosed::CreateUObject(this, &AWindowManager::NotifyWindowMoved));
	WidgetWindow->SetOnWindowClosed(FOnWindowClosed::CreateUObject(this, &AWindowManager::NotifyWindowClosed));

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

		if (InWindowObject->bIsFileDropEnabled == true)
		{
			FWindowsApplication* WindowsApplication = reinterpret_cast<FWindowsApplication*>(InWindowObject->WindowPtr.Get());
			WindowsApplication->RemoveMessageHandler(InWindowObject->DragDropHandler);
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

void AWindowManager::DetectHoveredWindow(bool bPrintDetected, FDelegateDetectHovered DelegateHovered)
{
	if (this->MAP_Windows.Num() > 0)
	{
		UPARAM(ref)TArray<UWindowObject*> ArrayWinObjects;
		this->MAP_Windows.GenerateValueArray(ArrayWinObjects);
		
		for (int32 WindowIndex = 0; WindowIndex < ArrayWinObjects.Num(); WindowIndex++)
		{
			if (IsValid(ArrayWinObjects[WindowIndex]) == true)
			{
				if (ArrayWinObjects[WindowIndex]->WindowPtr.IsValid() == true)
				{
					if (ArrayWinObjects[WindowIndex]->WindowPtr.ToSharedRef().Get().IsHovered() == true)
					{
						DelegateHovered.Execute(true, ArrayWinObjects[WindowIndex]);

						if (bPrintDetected == true)
						{
							GEngine->AddOnScreenDebugMessage(0, 10, FColor::Blue, ArrayWinObjects[WindowIndex]->WindowPtr.ToSharedRef().Get().GetTitle().ToString());
						}

						WindowIndex = 0;
						break;
					}
				}
			}
		}
	}
}

bool AWindowManager::SetAllWindowsOpacities(float NewOpacity)
{
	if (this->MAP_Windows.Num() > 0)
	{
		UPARAM(ref)TArray<UWindowObject*> ArrayWinObjects;
		this->MAP_Windows.GenerateValueArray(ArrayWinObjects);
		
		for (int32 WindowIndex = 0; WindowIndex < ArrayWinObjects.Num(); WindowIndex++)
		{
			bool OpacityReturn = UWindowSystemBPLibrary::SetWindowOpacity(ArrayWinObjects[WindowIndex], NewOpacity);

			if (OpacityReturn != true)
			{
				break;
				return false;
			}
		}

		return true;
	}

	else
	{
		return false;
	}
}