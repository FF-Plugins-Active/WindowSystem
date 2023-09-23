// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

THIRD_PARTY_INCLUDES_START

THIRD_PARTY_INCLUDES_END

#include "EachWindow_ImGui.generated.h"

UCLASS()
class WINDOWSYSTEM_API AEachWindow_ImGui : public AActor
{
	GENERATED_BODY()
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	
	// Sets default values for this actor's properties
	AEachWindow_ImGui();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
