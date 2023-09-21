// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

// Custom Includes.
#include "WindowEnums.h"

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

USTRUCT(BlueprintType)
struct WINDOWSYSTEM_API FFolderContent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FString Path = "";

	UPROPERTY(BlueprintReadOnly)
	FString Name = "";

	UPROPERTY(BlueprintReadOnly)
	bool bIsFile = false;
};

USTRUCT(BlueprintType)
struct WINDOWSYSTEM_API FContentArrayContainer
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	TArray<FFolderContent> OutContents;
};

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

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateOpenFile, FSelectedFiles, OutFileNames);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateSaveFile, bool, bIsSaveSuccessful, FString, OutFileName);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateSearch, bool, bIsSearchSuccessful, FString, ErrorCode, FContentArrayContainer, Out);

UCLASS()
class UWindowSystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Main Window Position", ToolTip = "Set Main Window Position", Keywords = "set, main, window, position"), Category = "Window System|Set|System")
	virtual void SetMainWindowPosition(FVector2D InNewPosition);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Main Window Title", Keywords = "get, window, title, main"), Category = "Window System|Get")
	static FText GetMainWindowTitle();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Select File From Dialog", ToolTip = "If you enable \"Allow Folder Selection\", extension filtering will be disabled. \nExtension filtering uses a String to String MAP variable. \nKey is description and value is extension's itself. You need to write like this without quotes \"*.extension\". \nIf one extension group has multiple extensions, you need to use \";\" after each one.", Keywords = "select, file, folder, dialog, windows, explorer"), Category = "Window System|File Dialog")
	static void SelectFileFromDialog(FDelegateOpenFile DelegateFileNames, const FString InDialogName, const FString InOkLabel, const FString InDefaultPath, TMap<FString, FString> InExtensions, int32 DefaultExtensionIndex, bool bIsNormalizeOutputs = true, bool bAllowFolderSelection = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Save File with Dialog", ToolTip = "Each extension group must have only one extension. \nIf that group has multiple variation, you should define one by one all of them if you need them. \nAlso you need to write them as \"*.extension\".", Keywords = "save, file, dialog, windows, explorer"), Category = "Window System|File Dialog")
	static void SaveFileDialog(FDelegateSaveFile DelegateSaveFile, const FString InDialogName, const FString InOkLabel, const FString InDefaultPath, TMap<FString, FString> InExtensions, int32 DefaultExtensionIndex, bool bIsNormalizeOutputs = true);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Folder Contents", ToolTip = "Description.", Keywords = "explorer, load, file, folder, content"), Category = "File Converters|File Dialog")
	static bool GetFolderContents(TArray<FFolderContent>& OutContents, FString& ErrorCode, FString InPath);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Search In Folder", ToolTip = "Description.", Keywords = "explorer, load, file, folder, content"), Category = "File Converters|File Dialog")
	static void SearchInFolder(FDelegateSearch DelegateSearch, FString InPath, FString InSearch, bool bSearchExact);

};