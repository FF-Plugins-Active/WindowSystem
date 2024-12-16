// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Custom Includes.
#include "WindowSystemBPLibrary.h"

#include "Viewport_Manager.generated.h"

UCLASS()
class WINDOWSYSTEM_API AViewport_Manager : public AActor
{
	GENERATED_BODY()
	
protected:
	
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed.
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	virtual void DetectLayoutChanges();
	virtual bool CompareViews(TMap<FVector2D, FVector2D> A, TMap<FVector2D, FVector2D> B);

	UFUNCTION()
	virtual void ChangeBackgroundOnNewPlayer(TArray<FPlayerViews> const& Out_Views);

	UCustomViewport* CustomViewport = nullptr;
	TMap<FVector2D, FVector2D> MAP_Views;

public:	

	// Sets default values for this actor's properties.
	AViewport_Manager();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnLayoutChanged(TArray<FPlayerViews> const& Out_Views);

	UPROPERTY(BlueprintReadWrite)
	UMaterialInterface* MAT_BG = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInterface* MAT_Brush = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FName CanvasName = TEXT("Canvas");

	UPROPERTY(BlueprintReadWrite)
	bool bEnableDebugLogs = true;

};
