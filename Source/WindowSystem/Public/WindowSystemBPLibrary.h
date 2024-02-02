// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

// UE Includes.
#include "Widgets/SWindow.h"				// Create Window.
#include "Widgets/SWidget.h"				// Add Widget to Window.
#include "Slate/WidgetRenderer.h"			// Take Screenshot of Window
#include "Runtime/UMG/Public/UMG.h"         // Take Screenshot of Window

// Custom Includes.
#include "CustomViewport.h"
#include "CustomGameView.h"

THIRD_PARTY_INCLUDES_START
#include "Windows/WindowsHWrapper.h"		// Necessary include.
#include "Windows/WindowsApplication.h"		// File Drag Drop Message Handler.
#include "shellapi.h"						// File Drag Drop Callback.
#include "dwmapi.h"							// Windows 11 Rounded Window Include.
#include <winreg.h>                         // Regedit access.
#include "winuser.h"						// Necessary include.
#include "Windows/MinWindows.h"				// Necessary include.	
THIRD_PARTY_INCLUDES_END

#include "WindowSystemBPLibrary.generated.h"

// Select file from dialog.
USTRUCT(BlueprintType)
struct WINDOWSYSTEM_API FSelectedFiles
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	bool IsSuccessfull = false;

	UPROPERTY(BlueprintReadOnly)
	bool IsFolder = false;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Strings;
};

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateOpenFile, FSelectedFiles, OutFileNames);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateSaveFile, bool, bIsSaveSuccessful, FString, OutFileName);

UCLASS()
class UWindowSystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Main Window Title", Keywords = "get, window, title, main"), Category = "Window System|Get")
	static FText GetMainWindowTitle();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Main Window Position", ToolTip = "Set Main Window Position", Keywords = "set, main, window, position"), Category = "Window System|Set|System")
	static void SetMainWindowPosition(FVector2D InNewPosition);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Select File From Dialog", ToolTip = "If you enable \"Allow Folder Selection\", extension filtering will be disabled. \nExtension filtering uses a String to String MAP variable. \nKey is description and value is extension's itself. You need to write like this without quotes \"*.extension\". \nIf one extension group has multiple extensions, you need to use \";\" after each one.", Keywords = "select, file, folder, dialog, windows, explorer"), Category = "Window System|File Dialog")
	static void SelectFileFromDialog(FDelegateOpenFile DelegateFileNames, const FString InDialogName, const FString InOkLabel, const FString InDefaultPath, TMap<FString, FString> InExtensions, int32 DefaultExtensionIndex, bool bIsNormalizeOutputs = true, bool bAllowFolderSelection = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Save File with Dialog", ToolTip = "Each extension group must have only one extension. \nIf that group has multiple variation, you should define one by one all of them if you need them. \nAlso you need to write them as \"*.extension\".", Keywords = "save, file, dialog, windows, explorer"), Category = "Window System|File Dialog")
	static void SaveFileDialog(FDelegateSaveFile DelegateSaveFile, const FString InDialogName, const FString InOkLabel, const FString InDefaultPath, TMap<FString, FString> InExtensions, int32 DefaultExtensionIndex, bool bIsNormalizeOutputs = true);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Posses Local Player", ToolTip = "If controller id is \"-1\", it will use main player's controller id which is probably 0. But else, it will use given index.", Keywords = "assign, new, controller, player, local"), Category = "Window System")
	static bool PossesLocalPlayer(const int32 PlayerId, const int32 ControllerId = -1);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Change Player View Size", ToolTip = "", Keywords = "change, player, view, viewport, size, position, ratio"), Category = "Window System")
	static bool ChangePlayerViewSize(const int32 PlayerId, FVector2D NewRatio, FVector2D NewOrigin);

};