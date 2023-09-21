// Fill out your copyright notice in the Description page of Project Settings.

#include "WindowManager.h"
#include "WindowSystemBPLibrary.h"

#include "EachWindow.h"

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
		this->Read_Color_Start();
	}

	Super::BeginPlay();
}

// Called when the game ends or when destroyed.
void AWindowManager::EndPlay(EEndPlayReason::Type Reason)
{
	this->RemoveDragDropHandlerFromMV();
	
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

void AWindowManager::Read_Color_Callback()
{
	if (GetKeyState(VK_LBUTTON) & 0x80)
	{
		HWND ScreenHandle = GetDesktopWindow();
		if (!ScreenHandle)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Screen Handle"));
			return;
		}

		HDC ScreenContext = GetDC(ScreenHandle);
		if (!ScreenContext)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Screen Context"));
			return;
		}

		POINT RawPos;
		bool GotCursorPos = GetCursorPos(&RawPos);
		if (!GotCursorPos)
		{
			UE_LOG(LogTemp, Warning, TEXT("Read Screen Color -> Error -> Got Cursor Pos : %d"), GetLastError());
			return;
		}

		COLORREF RawColor = GetPixel(ScreenContext, RawPos.x, RawPos.y);
		FLinearColor PositionColor;
		PositionColor.R = GetRValue(RawColor);
		PositionColor.G = GetGValue(RawColor);
		PositionColor.B = GetBValue(RawColor);
		PositionColor.A = 255;

		this->OnCursorPosColor(FVector2D(RawPos.x, RawPos.y), PositionColor);
		ReleaseDC(ScreenHandle, ScreenContext);
	}
}

// UFUNCTIONS.

bool AWindowManager::CloseAllWindows()
{
	if (this->MAP_Windows.Num() == 0)
	{
		return false;
	}

	UPARAM(ref)TArray<AEachWindow*> ArrayWinObjects;
	this->MAP_Windows.GenerateValueArray(ArrayWinObjects);

	for (int32 Index_Window = 0; Index_Window < ArrayWinObjects.Num(); Index_Window++)
	{
		ArrayWinObjects[Index_Window]->CloseWindow();
	}

	return true;
}

bool AWindowManager::Read_Color_Start()
{
	if (Timer_Color.IsValid())
	{
		return false;
	}

	GEngine->GetCurrentPlayWorld()->GetTimerManager().SetTimer(Timer_Color, this, &AWindowManager::Read_Color_Callback, 0.03, true);

	return true;
}

bool AWindowManager::Read_Color_Stop()
{
	if (!Timer_Color.IsValid())
	{
		return false;
	}

	GEngine->GetCurrentPlayWorld()->GetTimerManager().ClearTimer(Timer_Color);
	Timer_Color.Invalidate();

	return true;
}

bool AWindowManager::IsColorReading()
{
	return Timer_Color.IsValid();
}