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
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllowMainWindow = true;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Allow Main Window", Keywords = "allow, accept, main, window, viewport"), Category = "Window System|Set")
	virtual bool AllowMainWindow(FDroppedFileStruct InFile, FDroppedFileStruct& OutFile);

public:	
	// Sets default values for this actor's properties
	AWindowManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Window Movement Delegate
	void NotifyWindowMoved(const TSharedRef<SWindow>& Window);
	
	// Window Close Delegate
	void NotifyWindowClosed(const TSharedRef<SWindow>& Window);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnFileDrop(TArray<FDroppedFileStruct> const& OutMap);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowMoved(FName const& ClassName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowClosed(FName const& ClassName);

public:

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create New Window", Keywords = "create, new, window", AdvancedDisplay = "bForceVolatile, bPreserveAspectRatio, bSupportsMaximized, bSupportsMinimized, bSetMirrorWindow, bAllowFileDrop, InToolTip, DropColor"), Category = "Window System|Constructs")
	virtual bool CreateNewWindow(UPARAM(ref)UUserWidget*& InChildWidget, bool bIsTopMost, bool bHasClose, bool bForceVolatile, bool bPreserveAspectRatio, bool bMinimized, bool bSupportsMaximized, bool bSupportsMinimized, bool bSetMirrorWindow, bool bAllowFileDrop, FName InWindowTag, FText InWindowTitle, FText InToolTip, FVector2D WindowSize, FVector2D MinSize, FVector2D WindowPosition, FMargin InBorder, float InOpacity, FColor DropColor, UWindowObject*& OutWindowObject);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close Window", Keywords = "close, window"), Category = "Window System|Constructs")
	virtual bool CloseWindow(UPARAM(ref)UWindowObject*& InWindowObject);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close All Windows", Keywords = "close, all, window"), Category = "Window System|Constructs")
	virtual bool CloseAllWindows();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Detect Hovered Window", Keywords = "detect, hovered, window"), Category = "Window System|Check")
	virtual void DetectHoveredWindow(bool bPrintDetected, FDelegateDetectHovered DelegateHovered);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set All Windows Opacities", Keywords = "set, all, window, windows, opacity"), Category = "Window System|Set")
	virtual bool SetAllWindowsOpacities(float NewOpacity);

public:
	/*
	* We use this to record windows.
	* DO NOT CHANGE THIS IN EDITOR ! USE ONLY WITH BLUEPRINTS !
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<FName, UWindowObject*> MAP_Windows;

	// Constructed message handler subclass for main window.
	FDragDropHandler MW_DragDropHandler;
};
