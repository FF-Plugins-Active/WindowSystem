// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindowSystemBPLibrary.h"
#include "WindowManager.generated.h"

UCLASS()
class WINDOWSYSTEM_API AWindowManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWindowManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Window Movement Delegate
	void NotifyWindowMoved(const TSharedRef<SWindow>& Window);

	// Window Close Delegate
	void NotifyWindowClosed(const TSharedRef<SWindow>& Window);

	UFUNCTION(BlueprintImplementableEvent, meta = (Description = "Message came from WindowSystemBPLibrary.h \"OwnerActor->ProcessEvent(OwnerActor->FindFunction(FName(\"OnFileDrop\")), &OutArray);\""), Category = "Window System|Events")
	void OnFileDrop(TArray<FDroppedFileStruct> const& OutMap);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowMoved(FName const& ClassName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowClosed(FName const& ClassName);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bAllowMainWindow = true;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Drag Drop Handler To Main Window", Description = "It adds File Drag Drop message handler to main window. It is necessary to use if there another file drag drop supported window. Because they are child of this window.", Keywords = "add, main, window, viewport, handler"), Category = "Window System|Set")
	virtual void AddDragDropHandlerToMV();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Drag Drop Handler From Main Window", Keywords = "remove, main, window, viewport, handler"), Category = "Window System|Set")
	virtual void RemoveDragDropHandlerFromMV();

public:
	class FStyleContainer : public UObject
	{
	
	public:
		
		static FWindowStyle WindowStyle;
	};

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

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create New Window", Description = "If you disable \"Hide From Taskbar \", or enable \"Has Close\" there will be risk to remove widget accidently. So, use it with cautious.\nIf your window hide from taskbar, you need to use \"Bring Window Front\" function with some delay to see it.", Keywords = "create, new, window", AdvancedDisplay = "In_Window_Type, bForceVolatile, bPreserveAspectRatio, bSupportsMaximized, bSupportsMinimized, bSetMirrorWindow, bAllowFileDrop, bUseNativeBorder, InToolTip, TitleColor"), Category = "Window System|Constructs")
	virtual bool CreateNewWindow(UWindowObject*& OutWindowObject, UPARAM(ref)UUserWidget*& InChildWidget, EWindowTypeBp In_Window_Type = EWindowTypeBp::GameWindow, bool bIsTopMost = false, bool bHasClose = false, bool bForceVolatile = false, bool bPreserveAspectRatio = false, bool bMinimized = false, bool bSupportsMaximized = false, bool bSupportsMinimized = false, bool bSetMirrorWindow = false, bool bShowOnTaskBar = false, bool bUseNativeBorder = false, FName InWindowTag = NAME_None, FText InWindowTitle = INVTEXT("None"), FText InToolTip = INVTEXT("None"), FLinearColor TitleColor = FLinearColor::White, FVector2D WindowSize = FVector2D::ZeroVector, FVector2D MinSize = FVector2D::ZeroVector, FVector2D MaxSize = FVector2D::ZeroVector, FVector2D WindowPosition = FVector2D::ZeroVector, FMargin InBorder = FMargin());

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close Window", Keywords = "close, window"), Category = "Window System|Constructs")
	virtual bool CloseWindow(UPARAM(ref)UWindowObject*& InWindowObject);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close All Windows", Keywords = "close, all, window"), Category = "Window System|Constructs")
	virtual bool CloseAllWindows();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Detect Hovered Window", Keywords = "detect, hovered, window"), Category = "Window System|Check")
	virtual void DetectHoveredWindow(FDelegateDetectHovered DelegateHovered);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set File Drag Drop Support", Keywords = "set, file, drag, drop, support, child, window, windows"), Category = "Window System|Set")
	virtual bool SetFileDragDropSupport(UPARAM(ref)UWindowObject*& InWindowObject);

public:
	/*
	* We use this to record windows.
	* DO NOT CHANGE THIS IN EDITOR ! USE ONLY WITH BLUEPRINTS !
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FName, UWindowObject*> MAP_Windows;

	// Constructed message handler subclass for main window.
	FDragDropHandler DragDropHandler;
};