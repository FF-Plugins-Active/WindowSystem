// Fill out your copyright notice in the Description page of Project Settings.

#include "MainDragDrop.h"

// Sets default values
AMainDragDrop::AMainDragDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMainDragDrop::BeginPlay()
{
	Super::BeginPlay();
	
	HWND Window = reinterpret_cast<HWND>(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());
	DragAcceptFiles(Window, true);

	TSharedPtr<GenericApplication> Application = FSlateApplication::Get().GetPlatformApplication();

	if (Application.IsValid() == true)
	{
		DragDropHandler.OwnerActor = this;
		
		FWindowsApplication* WindowsApplication = reinterpret_cast<FWindowsApplication*>(Application.Get());
		WindowsApplication->AddMessageHandler(DragDropHandler);
	}
}

// Called when the game ends or when destroyed
void AMainDragDrop::EndPlay(EEndPlayReason::Type Reason)
{
	FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
	WindowsApplication->RemoveMessageHandler(DragDropHandler);

	Super::EndPlay(Reason);
}

// Called every frame
void AMainDragDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}