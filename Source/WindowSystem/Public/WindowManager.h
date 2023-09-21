// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Custom Includes.
#include "WindowSystemBPLibrary.h"

#include "WindowManager.generated.h"

class AEachWindow;

// File drag drop system.
USTRUCT(BlueprintType)
struct WINDOWSYSTEM_API FDroppedFileStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FString FilePath;

	UPROPERTY(BlueprintReadWrite)
	FVector2D DropLocation;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFolder = false;
};

UCLASS()
class WINDOWSYSTEM_API AWindowManager : public AActor
{
	GENERATED_BODY()

protected:

	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed.
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	virtual void AddDragDropHandlerToMV();

	virtual void RemoveDragDropHandlerFromMV();

	virtual void Read_Color_Callback();

	FTimerHandle Timer_Color;

public:

	// File Drag Drop Message Handler Subclass.
	class FDragDropHandler : public IWindowsMessageHandler
	{

	public:

		AActor* OwnerActor = nullptr;

		bool ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32& OutResult) override
		{
			// Drop System.
			AWindowManager* WindowManager = (AWindowManager*)this->OwnerActor;
			HWND MainWindowHandle;
			HDROP DropInfo = (HDROP)WParam;

			// File Path.
			char* DroppedFile;
			UINT DroppedFileCount = 0;

			// Drop Location.
			POINT DropLocation;

			// Out Structure.
			FDroppedFileStruct DropFileStruct;
			TArray<FDroppedFileStruct> OutArray;

			// Read Regedit To Get Windows Build Number.
			HKEY hKey;
			LONG Result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey);
			DWORD BufferSize;
			RegQueryValueEx(hKey, L"CurrentBuildNumber", 0, nullptr, NULL, &BufferSize);
			TCHAR* Buffer = (TCHAR*)malloc(BufferSize);
			RegQueryValueEx(hKey, L"CurrentBuildNumber", 0, nullptr, reinterpret_cast<LPBYTE>(Buffer), &BufferSize);
			int32 BuildNumber = FCString::Atoi(Buffer);

			switch (Message)
			{
			case WM_PAINT:

				if (BuildNumber >= 22000)
				{
					/*
						* Window Roundness Preference.
						* DWMWCP_DEFAULT = 0
						* DWMWCP_DONOTROUND = 1
						* DWMWCP_ROUND = 2
						* DWMWCP_ROUNDSMALL = 3
					*/
					DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
					DwmSetWindowAttribute(Hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
				}
				
				return true;

			case WM_DROPFILES:

				// If message sender window is main window and user not want to get files on it, return false.
				if (WindowManager->bAllowMainWindow == false)
				{
					MainWindowHandle = reinterpret_cast<HWND>(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());
					if (Hwnd == MainWindowHandle)
					{
						return false;
					}
				}

				DragQueryPoint(DropInfo, &DropLocation);

				DroppedFileCount = DragQueryFileA(DropInfo, 0xFFFFFFFF, NULL, NULL);
				for (UINT FileIndex = 0; FileIndex < DroppedFileCount; FileIndex++)
				{
					UINT PathSize = DragQueryFileA(DropInfo, FileIndex, NULL, 0);
					if (PathSize > 0)
					{
						DropFileStruct.DropLocation = FVector2D(DropLocation.x, DropLocation.y);

						DroppedFile = (char*)malloc(size_t(PathSize));
						DragQueryFileA(DropInfo, FileIndex, DroppedFile, PathSize + 1);

						if (GetFileAttributesA(DroppedFile) != FILE_ATTRIBUTE_DIRECTORY)
						{
							DropFileStruct.FilePath = DroppedFile;
							DropFileStruct.bIsFolder = false;
						}

						if (GetFileAttributesA(DroppedFile) == FILE_ATTRIBUTE_DIRECTORY)
						{
							DropFileStruct.FilePath = DroppedFile;
							DropFileStruct.bIsFolder = true;
						}

						OutArray.Add(DropFileStruct);
					}
				}

				WindowManager->OnFileDrop(OutArray);
				OutArray.Empty();

				DragFinish(DropInfo);

				return true;

			default:
				return false;
			}
		}
	};

public:
	// Sets default values for this actor's properties.
	AWindowManager();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

public:

	// Constructed message handler subclass for main window.
	FDragDropHandler DragDropHandler;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, AEachWindow*> MAP_Windows;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "", ExposeOnSpawn = "true"))
	bool bReadScreenColorAtStart = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "", ExposeOnSpawn = "true"))
	bool bAllowMainWindow = true;

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (Description = "Message came from WindowSystemBPLibrary.h \"OwnerActor->ProcessEvent(OwnerActor->FindFunction(FName(\"OnFileDrop\")), &OutArray);\""), Category = "Window System|Events")
	void OnFileDrop(TArray<FDroppedFileStruct> const& OutMap);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowClosed(FName const& WindowTag);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowMoved(AEachWindow* const& Window);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowHovered(AEachWindow* const& OutHovered);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnCursorPosColor(FVector2D const& Position, FLinearColor const& Color);

public:

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close All Windows", Keywords = "close, all, window"), Category = "Window System|Constructs")
	virtual bool CloseAllWindows();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Color Reading", Keywords = "color, reading, window, start"), Category = "Window System|Constructs")
	virtual bool Read_Color_Start();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Stop Color Reading", Keywords = "color, reading, window, stop"), Category = "Window System|Constructs")
	virtual bool Read_Color_Stop();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Check Is Color Reading", Keywords = "color, reading, window, check"), Category = "Window System|Constructs")
	virtual bool IsColorReading();

};