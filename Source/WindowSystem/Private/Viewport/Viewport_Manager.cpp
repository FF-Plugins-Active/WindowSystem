// Fill out your copyright notice in the Description page of Project Settings.

#include "Viewport/Viewport_Manager.h"

// Sets default values.
AViewport_Manager::AViewport_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned.
void AViewport_Manager::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(this->CustomViewport))
	{
		this->CustomViewport = Cast<UCustomViewport>(GEngine->GameViewport.Get());
	}

	this->DetectLayoutChanges();
}

// Called when the game ends or when destroyed.
void AViewport_Manager::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

// Called every frame.
void AViewport_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AViewport_Manager::DetectLayoutChanges()
{
	if (!this->CustomViewport)
	{
		return;
	}

	this->CustomViewport->DelegateNewLayout.AddUniqueDynamic(this, &ThisClass::ChangeBackgroundOnNewPlayer);
	this->CustomViewport->DelegateNewLayout.AddUniqueDynamic(this, &ThisClass::OnLayoutChanged);
}

bool AViewport_Manager::CompareViews(TMap<FVector2D, FVector2D> A, TMap<FVector2D, FVector2D> B)
{
	if (A.Num() != B.Num())
	{
		return false;
	}

	TArray<FVector2D> A_Keys;
	A.GenerateKeyArray(A_Keys);

	TArray<FVector2D> A_Values;
	A.GenerateValueArray(A_Values);

	TArray<FVector2D> B_Keys;
	B.GenerateKeyArray(B_Keys);

	TArray<FVector2D> B_Values;
	B.GenerateValueArray(B_Values);

	if (A_Keys == B_Keys && A_Values == B_Values)
	{
		return true;
	}

	else
	{
		return false;
	}
}

void AViewport_Manager::ChangeBackgroundOnNewPlayer(TArray<FPlayerViews> const& Out_Views)
{
	if (!this->CustomViewport)
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("Custom viewport is not valid !"));
		}
		
		return;
	}

	if (Out_Views.IsEmpty())
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("Views are empty !"));
		}
		
		return;
	}

	if (!IsValid(this->MAT_BG))
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("Background material is not valid !"));
		}
		
		return;
	}

	if (!IsValid(this->MAT_Brush))
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("Brush material is not valid !"));
		}
		
		return;
	}

	if (this->CanvasName.IsNone())
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("Canvas name is empty !"));
		}
		
		return;
	}

	FVector2D ViewportSize = FVector2D();
	this->CustomViewport->GetViewportSize(ViewportSize);

	if (ViewportSize == FVector2D(0.f))
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("Viewport size shouldn't be zero !"));
		}
		
		return;
	}

	TMap<FVector2D, FVector2D> Temp_Views;

	for (const FPlayerViews Each_View : Out_Views)
	{
		const FVector2D ActualPosition = ViewportSize * Each_View.Position;
		const FVector2D ActualSize = ViewportSize * Each_View.Size;

		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Warning, TEXT("Full Size = %s ; ActualPosition = %s, ActualSize = %s // UV Position = %s ; UV Size = %s"), *ViewportSize.ToString(), *ActualPosition.ToString(), *ActualSize.ToString(), *Each_View.Position.ToString(), *Each_View.Size.ToString());

		}

		Temp_Views.Add(ActualPosition, ActualSize);
	}

	if (Temp_Views.IsEmpty())
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("Actual size map is empty !"));
		}
		
		return;
	}

	if (this->CompareViews(this->MAP_Views, Temp_Views))
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Warning, TEXT("Views are not changed !"));
		}
		
		return;
	}

	this->MAP_Views = Temp_Views;
	const bool RetVal = UWindowSystemBPLibrary::SetBackgroundMaterial(this->MAT_BG, this->MAT_Brush, this->CanvasName, this->MAP_Views);

	if (RetVal)
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Warning, TEXT("Background changed successfully."));
		}
	}

	else
	{
		if (this->bEnableDebugLogs)
		{
			UE_LOG(LogTemp, Error, TEXT("There was a problem while changing background !"));
		}
	}
}