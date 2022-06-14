// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

// UE Includes.
#include "Widgets/SWindow.h"
#include "Widgets/SWidget.h"
#include "Runtime/UMG/Public/UMG.h"

#include "Windows/WindowsWindow.h"
#include "Windows/WindowsHWrapper.h"
#include "Windows/WindowsApplication.h"				// File Drag Drop Message Handler.
#include "Windows/AllowWindowsPlatformTypes.h" 
#include "Windows/HideWindowsPlatformTypes.h"

// C++ Includes.
#include <string>
#include <iostream>

// Windows Includes.
#include "shellapi.h"								// File Drag Drop Callback.

#include "WindowSystemBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

USTRUCT(BlueprintType)
struct FDroppedFileStruct
{
	GENERATED_BODY()

public:

		UPROPERTY(BlueprintReadWrite)
		FString FilePath;

		UPROPERTY(BlueprintReadWrite)
		FString SenderWindow;

		UPROPERTY(BlueprintReadWrite)
		FVector2D DropLocation;
};

UENUM(BlueprintType)
enum class EWindowState : uint8
{
	Minimized	UMETA(DisplayName = "Minimized"),
	Restored	UMETA(DisplayName = "Restored"),
	Maximized	UMETA(DisplayName = "Maximized"),
};
ENUM_CLASS_FLAGS(EWindowState)

// File Drag Drop Message Handler Subclass.
class FDragDropHandler : public IWindowsMessageHandler
{

public:

	// We will use this to print drop informations.
	AActor* OwnerActor;

	bool ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32& OutResult) override
	{
		if (Message == WM_DROPFILES)
		{
			// Drop System.
			HDROP DropInfo = (HDROP)WParam;
			char DroppedFile[MAX_PATH];

			// File Path.
			std::string Each_Path;

			// Drop Location.
			POINT DropLocation;
			FVector2D LocationVector;

			// Out Informations.
			FDroppedFileStruct DropFileStruct;
			TArray<FDroppedFileStruct> OutArray;

			for (int32 FileIndex = 0; DragQueryPoint(DropInfo, &DropLocation) && DragQueryFileA(DropInfo, FileIndex, (LPSTR)DroppedFile, sizeof(DroppedFile)); FileIndex++)
			{
				if (GetFileAttributesA(DroppedFile) != FILE_ATTRIBUTE_DIRECTORY)
				{
					TCHAR HandleName[256];
					GetWindowText(Hwnd, HandleName, 256);

					LocationVector.X = DropLocation.x;
					LocationVector.Y = DropLocation.y;

					Each_Path = DroppedFile;

					DropFileStruct.DropLocation = LocationVector;
					DropFileStruct.FilePath = Each_Path.c_str();
					DropFileStruct.SenderWindow = HandleName;

					OutArray.Add(DropFileStruct);
				}
			}

			OwnerActor->ProcessEvent(OwnerActor->FindFunction(FName("OnFileDrop")), &OutArray);

			DragFinish(DropInfo);
			OutArray.Empty();

			return true;
		}

		else
		{
			return false;
		}
	}
};

UCLASS(BlueprintType)
class WINDOWSYSTEM_API UWindowObject : public UObject
{
	GENERATED_BODY()

public:

	TSharedPtr<SWindow> WindowPtr;			// BlueprintType does not support TSharedRef. You can convert pointers to referances with .ToSharedRef()
	
	UPROPERTY(BlueprintReadOnly)
	UUserWidget* ContentWidget;				// We need to release widget before closing it and we can not get UUserWidget pointer from SWindow and its contents. So we just add it here.
	
	UPROPERTY(BlueprintReadOnly)
	FName WindowTag;						// We use this as WindowTag, FileDrop Window Name and FileDrop Class Name.
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsFileDropEnabled = false;		// If it is true, CloseWindow function will do additional tasks.

};

// Blueprint exposed delegate for GetViewportDragState and GetWindowDragState
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateHover, bool, bIsHovered);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateDetectHovered, bool, bIsHovered, UWindowObject*, OutHovered);

UCLASS()
class UWindowSystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Desktop Resolution", Keywords = "get, desktop, resolution"), Category = "Window System|Get|System")
	static FVector2D GetDesktopResolution();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Window Top Most", Keywords = "is, window, top, most"), Category = "Window System|Check")
	static void IsWindowTopMost(UPARAM(ref)UWindowObject*& InWindowObject, bool& bIsTopMost);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Window Hovered", Keywords = "is, window, hovered"), Category = "Window System|Check")
	static void IsWindowHovered(UPARAM(ref)UWindowObject*& InWindowObject, FDelegateHover DelegateHover);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bring Window Front", ToolTip = "It brings UE SWindow to front.", Keywords = "bring, window, front"), Category = "Window System|Set")
	static bool BringWindowFront(UPARAM(ref)UWindowObject*& InWindowObject, bool bFlashWindow);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window Opacity", Keywords = "set, window, opacity"), Category = "Window System|Set")
	static bool SetWindowOpacity(UPARAM(ref)UWindowObject*& InWindowObject, float NewOpacity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window State", Keywords = "set, window, state"), Category = "Window System|Set")
	static bool SetWindowState(UPARAM(ref)UWindowObject*& InWindowObject, EWindowState OutWindowState);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window Shape", ToolTip = "Description.", Keywords = "set, window, shape"), Category = "Window System|Set")
	static bool SetWindowShape(UPARAM(ref)UWindowObject*& InWindowObject, FMargin InExtend, float InDuration, float NewOpacity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window Position", ToolTip = "Description.", Keywords = "set, window, positon, location, move"), Category = "Window System|Set")
	static bool SetWindowPosition(UPARAM(ref)UWindowObject*& InWindowObject, FVector2D InNewPosition);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window Title", ToolTip = "Description.", Keywords = "set, window, title"), Category = "Window System|Set")
	static bool SetWindowTitle(UPARAM(ref)UWindowObject*& InWindowObject, FText InNewTitle);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Window State", Keywords = "get, window, state"), Category = "Window System|Get")
	static bool GetWindowState(UPARAM(ref)UWindowObject*& InWindowObject, EWindowState& OutWindowState);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Window Position", Keywords = "get, window, position, location"), Category = "Window System|Get")
	static bool GetWindowPosition(UPARAM(ref)UWindowObject*& InWindowObject, FVector2D& OutPosition);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Window Widget", Keywords = "get, window, widget, content"), Category = "Window System|Get")
	static bool GetWindowWidget(UPARAM(ref)UWindowObject*& InWindowObject, UUserWidget*& OutWidget);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Window Title", Keywords = "get, window, title"), Category = "Window System|Get")
	static bool GetWindowTitle(UPARAM(ref)UWindowObject*& InWindowObject, FText& OutWindowTitle);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Main Window Title", Keywords = "get, window, title, main"), Category = "Window System|Get")
	static FText GetMainWindowTitle();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Window Tag", Keywords = "get, window, tag"), Category = "Window System|Get")
	static bool GetWindowTag(UPARAM(ref)UWindowObject*& InWindowObject, FName& OutWindowTag);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Screenshot of Window", ToolTip = "Export To Disk functions should come after a delay node.", Keywords = "take, ss, screenshot, window"), Category = "Window System|Export")
	static bool TakeSSWindow(UPARAM(ref)UWindowObject*& InWindowObject, UTextureRenderTarget2D*& OutTextureRenderTarget2D);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Screenshot of Widget", ToolTip = "Export To Disk functions should come after a delay node.", Keywords = "take, ss, screenshot, widget"), Category = "Window System|Export")
	static bool TakeSSWidget(UUserWidget* InWidget, FVector2D InSize, UTextureRenderTarget2D*& OutTextureRenderTarget2D);

};