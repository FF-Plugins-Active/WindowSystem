// Fill out your copyright notice in the Description page of Project Settings.

#include "Window/WindowManager.h"

// Custom Includes.
#include "Window/EachWindow_SWindow.h"		// CloseAllWindows -> Destrow window actor.

// Sets default values.
AWindowManager::AWindowManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned.
void AWindowManager::BeginPlay()
{
	this->AddDragDropHandlerToMV();
	
	if (bReadScreenColorAtStart)
	{
		this->Read_Cursor_Infos();
		ActorPointer = (void*)this;
	}

	Super::BeginPlay();
}

// Called when the game ends or when destroyed.
void AWindowManager::EndPlay(EEndPlayReason::Type Reason)
{
	this->HoveredWindow = nullptr;

	this->RemoveDragDropHandlerFromMV();

	this->CloseAllWindows();
	
	if (MouseHook_Color)
	{
		UnhookWindowsHookEx(MouseHook_Color);

		ActorPointer = nullptr;
		free(ActorPointer);
	}

	Super::EndPlay(Reason);
}

// Called every frame.
void AWindowManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Protected Functions.

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

LRESULT AWindowManager::MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_LBUTTONDOWN)
	{
		if (!ActorPointer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Owner actor pointer is not valid !"));
			return CallNextHookEx(0, nCode, wParam, lParam);
		}

		AWindowManager* Owner = Cast<AWindowManager>((AWindowManager*)ActorPointer);

		if (!Owner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Owner actor is not valid !"));
			return CallNextHookEx(0, nCode, wParam, lParam);
		}

		HWND ScreenHandle = GetDesktopWindow();
		if (!ScreenHandle)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Screen Handle"));
			return CallNextHookEx(0, nCode, wParam, lParam);
		}

		HDC ScreenContext = GetDC(ScreenHandle);
		if (!ScreenContext)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Screen Context"));
			return CallNextHookEx(0, nCode, wParam, lParam);
		}

		POINT RawPos;
		bool GotCursorPos = GetCursorPos(&RawPos);
		if (!GotCursorPos)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Got Cursor Pos : %d"), GetLastError());
			return CallNextHookEx(0, nCode, wParam, lParam);
		}

		COLORREF RawColor = GetPixel(ScreenContext, RawPos.x, RawPos.y);
		FLinearColor PositionColor;
		PositionColor.R = GetRValue(RawColor);
		PositionColor.G = GetGValue(RawColor);
		PositionColor.B = GetBValue(RawColor);
		PositionColor.A = 255;

		Owner->OnCursorPosColor(FVector2D(RawPos.x, RawPos.y), PositionColor);
		ReleaseDC(ScreenHandle, ScreenContext);
	}

	return CallNextHookEx(0, nCode, wParam, lParam);
}

// UFUNCTIONS.

bool AWindowManager::CloseAllWindows()
{
	if (this->MAP_Windows.Num() == 0)
	{
		return false;
	}

	UPARAM(ref)TArray<AEachWindow_SWindow*> ArrayWinObjects;
	this->MAP_Windows.GenerateValueArray(ArrayWinObjects);

	for (int32 Index_Window = 0; Index_Window < ArrayWinObjects.Num(); Index_Window++)
	{
		if (IsValid(ArrayWinObjects[Index_Window]))
		{
			ArrayWinObjects[Index_Window]->Destroy();
		}
	}

	return true;
}

bool AWindowManager::Read_Cursor_Infos()
{
	MouseHook_Color = SetWindowsHookEx(WH_MOUSE_LL, AWindowManager::MouseHookCallback, NULL, 0);

	return true;
}

bool AWindowManager::ToggleWindowState(FName InTargetWindow, bool bFlashWindow)
{
	if (InTargetWindow.IsNone())
	{
		return false;
	}

	if (InTargetWindow.ToString().IsEmpty())
	{
		return false;
	}

	AEachWindow_SWindow* TargetWindow = *this->MAP_Windows.Find(InTargetWindow);

	if (!TargetWindow)
	{
		return false;
	}
	
	EWindowState WindowState = EWindowState::None;
	if (!TargetWindow->GetWindowState(WindowState))
	{
		return false;
	}

	TArray<AEachWindow_SWindow*> Array_Windows;
	this->MAP_Windows.GenerateValueArray(Array_Windows);

	switch (WindowState)
	{

	case EWindowState::None:
		
		return false;

	case EWindowState::Minimized:

		TargetWindow->SetWindowOpacity(1.f);
		TargetWindow->ToggleOpacity(false, true);

		TargetWindow->SetWindowState(EWindowState::Restored);
		TargetWindow->BringWindowFront(bFlashWindow);

		for (AEachWindow_SWindow* Each_Window : Array_Windows)
		{
			EWindowState Each_Window_State = EWindowState::None;
			Each_Window->GetWindowState(Each_Window_State);

			if (Each_Window_State != EWindowState::Minimized)
			{
				Each_Window->BringWindowFront(false);
			}
		}

		return true;

	case EWindowState::Restored:

		TargetWindow->SetWindowOpacity(1.f);
		TargetWindow->ToggleOpacity(false, true);

		if (TargetWindow->IsWindowTopMost())
		{
			TargetWindow->SetWindowState(EWindowState::Minimized);
		}

		else
		{
			TargetWindow->BringWindowFront(bFlashWindow);
		}

		for (AEachWindow_SWindow* Each_Window : Array_Windows)
		{
			EWindowState Each_Window_State = EWindowState::None;
			Each_Window->GetWindowState(Each_Window_State);

			if (Each_Window_State != EWindowState::Minimized)
			{
				Each_Window->BringWindowFront(false);
			}
		}

		return true;

	case EWindowState::Maximized:
		
		TargetWindow->SetWindowOpacity(1.f);
		TargetWindow->ToggleOpacity(false, true);

		if (TargetWindow->IsWindowTopMost())
		{
			TargetWindow->SetWindowState(EWindowState::Minimized);
		}

		else
		{
			TargetWindow->BringWindowFront(bFlashWindow);
		}

		for (AEachWindow_SWindow* Each_Window : Array_Windows)
		{
			EWindowState Each_Window_State = EWindowState::None;
			Each_Window->GetWindowState(Each_Window_State);

			if (Each_Window_State != EWindowState::Minimized)
			{
				Each_Window->BringWindowFront(false);
			}
		}

		return true;

	default:
		
		return false;
	}
}

bool AWindowManager::BringFrontOnHover(AEachWindow_SWindow* TargetWindow)
{
	if (!TargetWindow)
	{
		return false;
	}

	if (this->HoveredWindow == TargetWindow)
	{
		return false;
	}

	TArray<AEachWindow_SWindow*> Array_Windows;
	this->MAP_Windows.GenerateValueArray(Array_Windows);
	Array_Windows.Remove(TargetWindow);

	TargetWindow->SetWindowOpacity(1.f);
	TargetWindow->ToggleOpacity(false, true);
	
	this->HoveredWindow = TargetWindow;

	for (AEachWindow_SWindow* EachWindow : Array_Windows)
	{
		EWindowState WindowState = EWindowState::None;
		EachWindow->GetWindowState(WindowState);

		if (WindowState == EWindowState::Minimized)
		{
			continue;
		}

		EachWindow->ToggleOpacity(true, true);
		EachWindow->SetWindowOpacity(0.5f);
	}

	return true;
}