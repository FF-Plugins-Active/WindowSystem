// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowManager.h"

// Windows Includes.
#include "shellapi.h"
#include "Windows/WindowsWindow.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "Windows/AllowWindowsPlatformTypes.h"

// UE Includes.
#include "HAL/UnrealMemory.h"

// Global variables.
AWindowManager* Global_FileDropOuter;	// File Drop: Outer.
int FileDropOpacity = 1;				// File Drop: Handle Opacity: It should NOT be equal to ZERO (0). Because it will remove that layer. If it equal with 1, we will still have it also user won't be able to see it.

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

// CallBack for File Drop System.
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Drop System.
	HDROP DropInfo = (HDROP)wParam;
	char DroppedFile[MAX_PATH];

	// File Path.
	std::string temp_string;
	TArray<FString> Array_Paths;

	switch (msg)
	{
	case WM_CREATE:
		SetLayeredWindowAttributes(hwnd, 0, (255 * FileDropOpacity) / 100, LWA_ALPHA);
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_DROPFILES:
		for (int32 FileIndex = 0; DragQueryFileA(DropInfo, FileIndex, (LPSTR)DroppedFile, sizeof(DroppedFile)); FileIndex++)
		{
			if (GetFileAttributesA(DroppedFile) != FILE_ATTRIBUTE_DIRECTORY)
			{
				temp_string = DroppedFile;
				Array_Paths.Add(temp_string.c_str());
			}
		}

		Global_FileDropOuter->OnFileDrop(Array_Paths);
		DragFinish(DropInfo);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
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
		Global_FileDropOuter = this;

		// Step 1 = Create file drop class.
		const FString WindowClassString = InWindowTag.ToString() + TEXT("_FileDropClass");
		LPCWSTR FileDropClassName = TCHAR_TO_WCHAR(*WindowClassString);
		WNDCLASSEX FileDropWindowClass;
		
		FMemory::Memzero(&FileDropWindowClass, sizeof(FileDropWindowClass));

		FileDropWindowClass.cbSize = sizeof(WNDCLASSEX);
		FileDropWindowClass.style = 0;
		FileDropWindowClass.lpfnWndProc = WndProc;
		FileDropWindowClass.cbClsExtra = 0;
		FileDropWindowClass.cbWndExtra = 0;
		FileDropWindowClass.hInstance = hInstance;
		FileDropWindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		FileDropWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		FileDropWindowClass.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(DropColor.R, DropColor.G, DropColor.B)));
		FileDropWindowClass.lpszMenuName = NULL;
		FileDropWindowClass.lpszClassName = FileDropClassName;
		FileDropWindowClass.hIconSm = LoadIcon(FileDropWindowClass.hInstance, IDI_APPLICATION);
		
		RegisterClassEx(&FileDropWindowClass);

		// Step 2 = Create file drop window. We will work on this.
		//MSG WindowMessage;
		
		const FString WindowNameString = InWindowTag.ToString() + TEXT("_FileDropWindow");
		LPCWSTR FileDropWindowName = TCHAR_TO_WCHAR(*WindowNameString);
		HWND FileDropWindowHandle;

		FileDropWindowHandle = CreateWindowEx
		(
			WS_EX_ACCEPTFILES | WS_EX_LAYERED | WS_EX_LAYERED,									// Extended possibilites for variation.
			FileDropClassName,
			FileDropWindowName,
			WS_CHILD | WS_VISIBLE,																// Visible child window.
			InBorder.Left * 2,																	// X Position.
			(WidgetWindow.ToSharedRef().Get().GetTitleBarSize().Get() + InBorder.Top) * 2,		// Y Position.
			WindowSize.X - (InBorder.Left + InBorder.Right) * 3,								// X Size
			WindowSize.Y - (InBorder.Top + InBorder.Bottom) * 3,								// Y Size
			WidgetWindowHandle,																	// The window is a child-window to SWindow.
			(HMENU)NULL,																		// No menu.
			hInstance,																			// Program Instance handler.
			NULL																				// No Window Creation data.
		);

		// Step 3 = Visualize file drop window.
		ShowWindow(FileDropWindowHandle, SW_SHOWDEFAULT);
		UpdateWindow(FileDropWindowHandle);

		// Step 4 = Set file drop variables in window BP object.
		WindowObject->FileDropWindowClass = FileDropWindowClass;
		WindowObject->FileDropWindowHandle = FileDropWindowHandle;
	}

	WindowObject->bIsFileDropEnabled = bAllowFileDrop;
	WindowObject->WindowPtr = WidgetWindow;
	WindowObject->ContentWidget = InChildWidget;
	WindowObject->Border = InBorder;
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
			UnregisterClassW(InWindowObject->FileDropWindowClass.lpszClassName, hInstance);
			DestroyWindow(InWindowObject->FileDropWindowHandle);
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

bool AWindowManager::SetFileDropOpacity(int32 InFileDropOpacity)
{
	if (InFileDropOpacity > 0 == true)
	{
		FileDropOpacity = InFileDropOpacity;
		return true;
	}

	else
	{
		return false;
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