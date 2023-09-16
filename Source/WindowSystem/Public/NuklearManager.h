// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

THIRD_PARTY_INCLUDES_START
// Nuklear Includes.
#define NK_IMPLEMENTATION
#include "nuklear.h"
THIRD_PARTY_INCLUDES_END

#include "NuklearManager.generated.h"

UCLASS()
class WINDOWSYSTEM_API ANuklearManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANuklearManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};