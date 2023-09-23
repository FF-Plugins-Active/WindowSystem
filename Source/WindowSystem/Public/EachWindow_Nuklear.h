// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EachWindow_Nuklear.generated.h"

UCLASS()
class WINDOWSYSTEM_API AEachWindow_Nuklear : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEachWindow_Nuklear();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
