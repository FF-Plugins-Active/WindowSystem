// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Custom Includes.
#include "WindowSystemBPLibrary.h"
#include "DragDropHandler.h"

#include "WindowManager.generated.h"

class AEachWindow_SWindow;

// File drag drop system.
USTRUCT(BlueprintType)
struct WINDOWSYSTEM_API FDroppedFileStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FString FilePath;

	UPROPERTY(BlueprintReadWrite)
	FVector2D DropLocation = FVector2D();

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

protected:

	// Constructed message handler subclass for main window.
	FDragDropHandler DragDropHandler;

	virtual void AddDragDropHandlerToMV();

	virtual void RemoveDragDropHandlerFromMV();

protected:

	virtual void DetectLayoutChanges();

protected:

	HHOOK MouseHook_Color;

	static inline void* ActorPointer;

	static LRESULT MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);

public:
	
	// Sets default values for this actor's properties.
	AWindowManager();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, AEachWindow_SWindow*> MAP_Windows;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "", ExposeOnSpawn = "true"))
	bool bReadScreenColorAtStart = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "It allows main window to support file drag drop.", ExposeOnSpawn = "true"))
	bool bAllowMainWindow = true;

	UFUNCTION(BlueprintImplementableEvent, meta = (Description = "Message came from WindowSystemBPLibrary.h \"OwnerActor->ProcessEvent(OwnerActor->FindFunction(FName(\"OnFileDrop\")), &OutArray);\""), Category = "Window System|Events")
	void OnFileDrop(TArray<FDroppedFileStruct> const& OutMap);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowClosed(FName const& WindowTag);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowMoved(AEachWindow_SWindow* const& Window);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnWindowHovered(AEachWindow_SWindow* const& OutHovered);

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnCursorPosColor(FVector2D const& Position, FLinearColor const& Color);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLayoutChanged(TArray<FPlayerViews> const& Array_Views);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close All Windows", Keywords = "close, all, window"), Category = "Window System")
	virtual bool CloseAllWindows();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Cursor Infos", ToolTip = "It will give cursor position and color under cursor.", Keywords = "cursor, mouse, color, pixel, position, location"), Category = "Window System")
	virtual bool Read_Cursor_Infos();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Cursor Infos", ToolTip = "It will give cursor position and color under cursor.", Keywords = "cursor, mouse, color, pixel, position, location"), Category = "Window System")
	virtual bool ToggleWindowState(FName InTargetWindow);

};