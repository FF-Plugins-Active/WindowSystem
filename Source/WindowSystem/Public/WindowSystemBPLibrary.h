// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

// Windows Includes.
#include <Windows/WindowsHWrapper.h>
#include "winuser.h"

// UE Includes.
#include "Widgets/SWindow.h"
#include "Widgets/SWidget.h"
#include "Runtime/UMG/Public/UMG.h"

// C++ Includes.
#include <string>
#include <iostream>

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

UENUM(BlueprintType)
enum class EWindowState : uint8
{
	Minimized	UMETA(DisplayName = "Minimized"),
	Restored	UMETA(DisplayName = "Restored"),
	Maximized	UMETA(DisplayName = "Maximized"),
};
ENUM_CLASS_FLAGS(EWindowState)


UCLASS(BlueprintType)
class WINDOWSYSTEM_API UWindowObject : public UObject
{
	GENERATED_BODY()

public:

	// SWindow Variables.
	TSharedPtr<SWindow> WindowPtr;		// BlueprintType does not support TSharedRef. You can convert pointers to referances with .ToSharedRef()
	UUserWidget* ContentWidget;			// We need to release widget before closing it and we can not get UUserWidget pointer from SWindow and its contents. So we just add it here.

	// FileDrop Variables.
	bool bIsFileDropEnabled;			// If it is true, CloseWindow function will do additional tasks.
	WNDCLASSEX FileDropWindowClass;
	HWND FileDropWindowHandle;

	// Misc Variables.
	FMargin Border;						// We use this to calculate file drop window size.
	FName WindowTag;					// We use this as WindowTag, FileDrop Window Name and FileDrop Class Name.
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

	// Helper function for File Drop System.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set File Drop Size", ToolTip = "This is a helper function for File Drop system. DO NOT MESS WITH IT, if you don't know what you are doing.", Keywords = "set, file, drop, size"), Category = "Window System|Set|File Drop")
	static bool SetFileDropSize(UPARAM(ref)UWindowObject*& InWindowObject, int32 X_Multiplier, int32 Y_Multiplier);

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

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Window Tag", Keywords = "get, window, tag"), Category = "Window System|Get")
	static bool GetWindowTag(UPARAM(ref)UWindowObject*& InWindowObject, FName& OutWindowTag);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Screenshot of Window", ToolTip = "Export To Disk functions should come after a delay node.", Keywords = "take, ss, screenshot, window"), Category = "Window System|Export")
	static bool TakeSSWindow(UPARAM(ref)UWindowObject*& InWindowObject, UTextureRenderTarget2D*& OutTextureRenderTarget2D);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Screenshot of Widget", ToolTip = "Export To Disk functions should come after a delay node.", Keywords = "take, ss, screenshot, widget"), Category = "Window System|Export")
	static bool TakeSSWidget(UUserWidget* InWidget, FVector2D InSize, UTextureRenderTarget2D*& OutTextureRenderTarget2D);

};