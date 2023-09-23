// Fill out your copyright notice in the Description page of Project Settings.

#include "EachWindow_ImGui.h"

THIRD_PARTY_INCLUDES_START

THIRD_PARTY_INCLUDES_END

// Sets default values
AEachWindow_ImGui::AEachWindow_ImGui()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEachWindow_ImGui::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEachWindow_ImGui::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEachWindow_ImGui::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}