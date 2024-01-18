// Fill out your copyright notice in the Description page of Project Settings.

#include "WindowManager.h"

// Custom Includes.
#include "EachWindow_SWindow.h"		// CloseAllWindows -> Destrow window actor.
#include "CustomViewport.h"

void* Global_ActorPointer = nullptr;

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
		this->Read_Color();
		Global_ActorPointer = (void*)this;
	}

	this->DetectLayoutChanges();
	Super::BeginPlay();
}

// Called when the game ends or when destroyed.
void AWindowManager::EndPlay(EEndPlayReason::Type Reason)
{
	this->RemoveDragDropHandlerFromMV();

	this->CloseAllWindows();
	
	if (MouseHook_Color)
	{
		UnhookWindowsHookEx(MouseHook_Color);

		Global_ActorPointer = nullptr;
		free(Global_ActorPointer);
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

bool AWindowManager::Read_Color()
{
	auto MouseHookCallback = [](int nCode, WPARAM wParam, LPARAM lParam)->LRESULT
		{
			if (wParam == WM_LBUTTONDOWN)
			{
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

				if (!Global_ActorPointer)
				{
					UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Owner actor pointer is not valid !"));
					return CallNextHookEx(0, nCode, wParam, lParam);
				}

				AWindowManager* Owner = Cast<AWindowManager>((AWindowManager*)Global_ActorPointer);

				if (!Owner)
				{
					UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Owner actor is not valid !"));
					return CallNextHookEx(0, nCode, wParam, lParam);
				}

				Owner->OnCursorPosColor(FVector2D(RawPos.x, RawPos.y), PositionColor);
				ReleaseDC(ScreenHandle, ScreenContext);
			}

			return CallNextHookEx(0, nCode, wParam, lParam);
		};
	
	MouseHook_Color = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, NULL, 0);

	return true;
}

void AWindowManager::DetectLayoutChanges()
{
	UCustomViewport* CustomViewport = Cast<UCustomViewport>(GEngine->GameViewport.Get());

	if (!CustomViewport)
	{
		return;
	}

	CustomViewport->DelegateNewLayout.AddUniqueDynamic(this, &ThisClass::OnLayoutChanged);
}