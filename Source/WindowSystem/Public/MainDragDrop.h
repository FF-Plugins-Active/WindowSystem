// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindowSystemBPLibrary.h"

// UE Includes.
#include "Windows/WindowsApplication.h"

#include "MainDragDrop.generated.h"

class FMessageHandler : public IWindowsMessageHandler
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
			TMap<FString, FVector2D> Array_Out;
			
			for (int32 FileIndex = 0; DragQueryPoint(DropInfo, &DropLocation) && DragQueryFileA(DropInfo, FileIndex, (LPSTR)DroppedFile, sizeof(DroppedFile)); FileIndex++)
			{
				if (GetFileAttributesA(DroppedFile) != FILE_ATTRIBUTE_DIRECTORY)
				{
					LocationVector.X = DropLocation.x;
					LocationVector.Y = DropLocation.y;
					
					Each_Path = DroppedFile;
					
					Array_Out.Add(Each_Path.c_str(), LocationVector);
				}
			}
			
			OwnerActor->ProcessEvent(OwnerActor->FindFunction(FName("OnFileDrop")), &Array_Out);

			DragFinish(DropInfo);
			Array_Out.Empty();

			return true;
		}

		else
		{
			return false;
		}
	}
};

UCLASS()
class WINDOWSYSTEM_API AMainDragDrop : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called when the game ends or when destroyed
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:	
	// Sets default values for this actor's properties
	AMainDragDrop();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Constructed message handler subclass.
	FMessageHandler DragDropHandler;

	UFUNCTION(BlueprintImplementableEvent, Category = "Window System|Events")
	void OnFileDrop(TMap<FString, FVector2D> const& Array_Out);
};