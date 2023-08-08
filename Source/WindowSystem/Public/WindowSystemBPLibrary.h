// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

// UE Includes.
#include "Widgets/SWindow.h"
#include "Widgets/SWidget.h"
#include "Runtime/UMG/Public/UMG.h"

// Windows Includes.
THIRD_PARTY_INCLUDES_START
#include "Windows/WindowsHWrapper.h"
#include "Windows/WindowsApplication.h"				// File Drag Drop Message Handler.
#include "shellapi.h"								// File Drag Drop Callback.
#include "dwmapi.h"									// Windows 11 Rounded Window Include.
#include <winreg.h>                                 // Regedit access.
#include "winuser.h"
#include "Windows/MinWindows.h"

// C++ Includes.
#include <string>
#include <iostream>
THIRD_PARTY_INCLUDES_END

#include "WindowSystemBPLibrary.generated.h"

USTRUCT(BlueprintType)
struct FDroppedFileStruct
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

UENUM(BlueprintType)
enum class EWindowState : uint8
{
	Minimized	UMETA(DisplayName = "Minimized"),
	Restored	UMETA(DisplayName = "Restored"),
	Maximized	UMETA(DisplayName = "Maximized"),
};
ENUM_CLASS_FLAGS(EWindowState)

UENUM(BlueprintType)
enum class EWindowTypeBp : uint8
{
	Normal				UMETA(DisplayName = "Normal"),
	Menu				UMETA(DisplayName = "Menu"),
	ToolTip				UMETA(DisplayName = "ToolTip"),
	Notification		UMETA(DisplayName = "Notification"),
	CursorDecorator		UMETA(DisplayName = "CursorDecorator"),
	GameWindow			UMETA(DisplayName = "GameWindow"),
};
ENUM_CLASS_FLAGS(EWindowTypeBp)

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

	UPROPERTY(BlueprintReadOnly)
	bool bShowOnTaskBar = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsTransparent = false;
};

// Blueprint exposed delegate for GetViewportDragState and GetWindowDragState
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateHover, bool, bIsHovered);

UCLASS()
class UWindowSystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Window Top Most", Keywords = "is, window, top, most"), Category = "Window System|Check")
	static bool IsWindowTopMost(UPARAM(ref)UWindowObject*& InWindowObject, bool bUseNative = true);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Window Hovered", Keywords = "is, window, hovered"), Category = "Window System|Check")
	static void IsWindowHovered(UPARAM(ref)UWindowObject*& InWindowObject, FDelegateHover DelegateHover);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bring Window Front", ToolTip = "It brings UE SWindow to front.", Keywords = "bring, window, front"), Category = "Window System|Set")
	static bool BringWindowFront(UPARAM(ref)UWindowObject*& InWindowObject, bool bFlashWindow);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Toggle Top Most Option", ToolTip = "Description.", Keywords = "set, window, positon, location, move"), Category = "Window System|Set")
	static bool ToggleTopMostOption(UPARAM(ref)UWindowObject*& InWindowObject);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Toggle Show On Task Bar", Keywords = "toggle, show, taskbar, hide"), Category = "Window System|Set")
	static bool ToggleShowOnTaskBar(UPARAM(ref)UWindowObject*& InWindowObject, bool bShowOnTaskBar);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Toggle Opacity", Keywords = "set, all, window, windows, opacity"), Category = "Window System|Set")
	static bool ToggleOpacity(UPARAM(ref)UWindowObject*& InWindowObject, bool bEnable, bool bPassDragDrop);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window Opacity", Keywords = "set, window, opacity"), Category = "Window System|Set")
	static bool SetWindowOpacity(UPARAM(ref)UWindowObject*& InWindowObject, float NewOpacity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window State", Keywords = "set, window, state"), Category = "Window System|Set")
	static bool SetWindowState(UPARAM(ref)UWindowObject*& InWindowObject, EWindowState OutWindowState);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window Shape", ToolTip = "Description.", Keywords = "set, window, shape"), Category = "Window System|Set")
	static bool SetWindowShape(UPARAM(ref)UWindowObject*& InWindowObject, FMargin InExtend, float InDuration, float NewOpacity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Window Position", ToolTip = "Description.", Keywords = "set, window, positon, location, move"), Category = "Window System|Set")
	static bool SetWindowPosition(UPARAM(ref)UWindowObject*& InWindowObject, FVector2D InNewPosition);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Main Window Position", ToolTip = "Set Main Window Position", Keywords = "set, main, window, position"), Category = "Window System|Set|System")
	static void SetMainWindowPosition(FVector2D InNewPosition);

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

};