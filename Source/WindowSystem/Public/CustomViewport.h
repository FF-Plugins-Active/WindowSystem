// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameViewportClient.h"
#include "Engine/ViewportSplitScreen.h"

#include "CustomViewport.generated.h"

UENUM(BlueprintType)
namespace ECustomSplitScreenType
{
    enum Type
    {
        None,
        TwoPlayer       = ESplitScreenType::SplitTypeCount,
        ThreePlayer     = ESplitScreenType::SplitTypeCount,
        SplitTypeCount
    };
}

UCLASS()
class WINDOWSYSTEM_API UCustomViewport : public UGameViewportClient
{
	GENERATED_BODY()
	
public:

	UCustomViewport();

    virtual void UpdateActiveSplitscreenType() override;

    virtual void LayoutPlayers() override;

};
