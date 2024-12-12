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
    FVector2D Size = FVector2D();

    UPROPERTY(BlueprintReadOnly)
    FVector2D Position = FVector2D();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateNewLayout, const TArray<FPlayerViews>&, Array_Views);

UCLASS()
class WINDOWSYSTEM_API UCustomViewport : public UGameViewportClient
{
	GENERATED_BODY()

protected:

    /*
    * Material needs to be User Interface type.
    * TODO: We need to calculate gameview maskings.
    */
    UMaterialInterface* BG_Material = nullptr;

public:

	UCustomViewport();

    virtual void Tick(float DeltaTime) override;

    virtual void UpdateActiveSplitscreenType() override;

    virtual void LayoutPlayers() override;

    virtual void Draw(FViewport* In_Viewport, FCanvas* In_SceneCanvas) override;

    FDelegateNewLayout DelegateNewLayout;

    // This is customized version of UGameViewportClient::SSSwapControllers which works on Shipping Builds.
    virtual bool PossesLocalPlayer(const int32 PlayerId, const int32 ControllerId = -1);

    virtual bool ChangePlayerViewSize(const int32 PlayerId, FVector2D NewRatio, FVector2D NewOrigin);

    virtual bool SetBackgrounMaterial(UMaterialInterface* In_Material);
    virtual UMaterialInterface* GetBackgroundMaterial();

};