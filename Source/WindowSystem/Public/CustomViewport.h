// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameViewportClient.h"
#include "Engine/ViewportSplitScreen.h"

#include "CustomViewport.generated.h"

USTRUCT(BlueprintType)
struct WINDOWSYSTEM_API FPlayerViews
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly)
    FVector2D Size;

    UPROPERTY(BlueprintReadOnly)
    FVector2D Position;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateNewLayout, const TArray<FPlayerViews>&, Array_Views);

UCLASS()
class WINDOWSYSTEM_API UCustomViewport : public UGameViewportClient
{
	GENERATED_BODY()
	
public:

	UCustomViewport();

    virtual void Tick(float DeltaTime) override;

    virtual void UpdateActiveSplitscreenType() override;

    virtual void LayoutPlayers() override;

    // This is customized version of UGameViewportClient::SSSwapControllers which works on Shipping Builds.
    virtual bool PossesLocalPlayer(const int32 PlayerId, const int32 ControllerId = -1);

    FDelegateNewLayout DelegateNewLayout;

protected:

    int32 LastPlayerCount = 0;

};