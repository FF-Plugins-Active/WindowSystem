// Fill out your copyright notice in the Description page of Project Settings.

#include "Viewport/CustomViewport.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"

UCustomViewport::UCustomViewport() : Super(FObjectInitializer::Get())
{
    MaxSplitscreenPlayers = 4;
}

void UCustomViewport::UpdateActiveSplitscreenType()
{
    Super::UpdateActiveSplitscreenType();
}

void UCustomViewport::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void UCustomViewport::LayoutPlayers()
{
    UpdateActiveSplitscreenType();
    
    const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();

    // Initialize the players
    const TArray<ULocalPlayer*>& PlayerList = GetOuterUEngine()->GetGamePlayers(this);

    if (LastPlayerCount == PlayerList.Num())
    {
        return;
    }

    LastPlayerCount = PlayerList.Num();
    TArray<FPlayerViews> Array_Views;

    FVector2D ViewportSize;
    this->GetViewportSize(ViewportSize);

    //UE_LOG(LogTemp, Warning, TEXT("Viewport Size Log = %s"), *ViewportSize.ToString());

    if (PlayerList.Num() == 1)
    {
        PlayerList[0]->Size.X = 0.9;
        PlayerList[0]->Size.Y = 0.9;
        PlayerList[0]->Origin.X = 0.05;
        PlayerList[0]->Origin.Y = 0.05;
        
        DelegateNewLayout.Broadcast(Array_Views);

        return;
    }

    else if (PlayerList.Num() == 2)
    {
        // Player 1 = Right

        PlayerList[0]->Size.X = 0.425;
        PlayerList[0]->Size.Y = 0.9;
        PlayerList[0]->Origin.X = 0.525;
        PlayerList[0]->Origin.Y = 0.05;

        // Player 2 = Left

        PlayerList[1]->Size.X = 0.425;
        PlayerList[1]->Size.Y = 0.9;
        PlayerList[1]->Origin.X = 0.05;
        PlayerList[1]->Origin.Y = 0.05;

        DelegateNewLayout.Broadcast(Array_Views);

        return;
    }

    else if (PlayerList.Num() == 3)
    {
        // Player 1 = Right

        PlayerList[0]->Size.X = 0.425;
        PlayerList[0]->Size.Y = 0.9;
        PlayerList[0]->Origin.X = 0.525;
        PlayerList[0]->Origin.Y = 0.05;

        // Player 2 = Top Left

        PlayerList[1]->Size.X = 0.425;
        PlayerList[1]->Size.Y = 0.425;
        PlayerList[1]->Origin.X = 0.05;
        PlayerList[1]->Origin.Y = 0.05;

        //Player 3 = Bottom Left

        PlayerList[2]->Size.X = 0.425;
        PlayerList[2]->Size.Y = 0.425;
        PlayerList[2]->Origin.X = 0.05;
        PlayerList[2]->Origin.Y = 0.525;

        DelegateNewLayout.Broadcast(Array_Views);

        return;
    }

    else if (PlayerList.Num() == 4)
    {
        // Player 1 = Bottom Right

        PlayerList[0]->Size.X = 0.425;
        PlayerList[0]->Size.Y = 0.425;
        PlayerList[0]->Origin.X = 0.525;
        PlayerList[0]->Origin.Y = 0.525;

        // Player 2 = Top Right

        PlayerList[1]->Size.X = 0.425;
        PlayerList[1]->Size.Y = 0.425;
        PlayerList[1]->Origin.X = 0.525;
        PlayerList[1]->Origin.Y = 0.05;

        // Player 3 = Top Left

        PlayerList[2]->Size.X = 0.425;
        PlayerList[2]->Size.Y = 0.425;
        PlayerList[2]->Origin.X = 0.05;
        PlayerList[2]->Origin.Y = 0.05;

        // Player 4 = Bottom Left

        PlayerList[3]->Size.X = 0.425;
        PlayerList[3]->Size.Y = 0.425;
        PlayerList[3]->Origin.X = 0.05;
        PlayerList[3]->Origin.Y = 0.525;

        DelegateNewLayout.Broadcast(Array_Views);

        return;
    }

    for (int32 PlayerIdx = 0; PlayerIdx < PlayerList.Num(); PlayerIdx++)
    {
        if (SplitType < SplitscreenInfo.Num() && PlayerIdx < SplitscreenInfo[SplitType].PlayerData.Num())
        {
            PlayerList[PlayerIdx]->Size.X = SplitscreenInfo[SplitType].PlayerData[PlayerIdx].SizeX;
            PlayerList[PlayerIdx]->Size.Y = SplitscreenInfo[SplitType].PlayerData[PlayerIdx].SizeY;
            
            PlayerList[PlayerIdx]->Origin.X = SplitscreenInfo[SplitType].PlayerData[PlayerIdx].OriginX;
            PlayerList[PlayerIdx]->Origin.Y = SplitscreenInfo[SplitType].PlayerData[PlayerIdx].OriginY;
        }

        else
        {
            PlayerList[PlayerIdx]->Size.X = 0.f;
            PlayerList[PlayerIdx]->Size.Y = 0.f;
            
            PlayerList[PlayerIdx]->Origin.X = 0.f;
            PlayerList[PlayerIdx]->Origin.Y = 0.f;
        }
    }
}

void UCustomViewport::Draw(FViewport* In_Viewport, FCanvas* In_SceneCanvas)
{
    if (!In_Viewport || !In_SceneCanvas)
    {
        return;
    }

    if (this->BG_Type < 0 || this->BG_Type > 2)
    {
        return;
    }

    switch (this->BG_Type)
    {
        case 0: 
        {
            const FVector2D ViewportSize(In_Viewport->GetSizeXY());
            FCanvasTileItem TileItem(FVector2D(0, 0), ViewportSize, this->BG_Color);
            TileItem.BlendMode = SE_BLEND_Opaque;
            In_SceneCanvas->DrawItem(TileItem);

            break;
        }

        case 1:
        {
            const FVector2D ViewportSize(In_Viewport->GetSizeXY());
            const FLinearColor TopLeft = this->GradientStart;
            const FLinearColor BottomRight = this->GradientEnd;

            const FVector2D PointA1(0, 0);
            const FVector2D PointB1(ViewportSize.X, 0);
            const FVector2D PointC1(0, ViewportSize.Y);

            const FVector2D PointA2(ViewportSize.X, 0);
            const FVector2D PointB2(ViewportSize.X, ViewportSize.Y);
            const FVector2D PointC2(0, ViewportSize.Y);

            FCanvasTriangleItem Triangle1(PointA1, PointB1, PointC1, nullptr);
            Triangle1.SetColor(TopLeft);
            Triangle1.BlendMode = SE_BLEND_Translucent;

            FCanvasTriangleItem Triangle2(PointA2, PointB2, PointC2, nullptr);
            Triangle2.SetColor(BottomRight);
            Triangle2.BlendMode = SE_BLEND_Translucent;

            In_SceneCanvas->DrawItem(Triangle1);
            In_SceneCanvas->DrawItem(Triangle2);

            break;
        }

        case 2:
        {
            if (!IsValid(this->BG_Texture))
            {
                break;
            }

            const FVector2D ViewportSize(In_Viewport->GetSizeXY());
            In_SceneCanvas->DrawTile(0, 0, ViewportSize.X, ViewportSize.Y, 0, 0, 1, 1, FLinearColor::White, this->BG_Texture->GetResource());
            
            break;
        }
        
        default:
        {
            const FVector2D ViewportSize(In_Viewport->GetSizeXY());
            FCanvasTileItem TileItem(FVector2D(0, 0), ViewportSize, this->BG_Color);
            TileItem.BlendMode = SE_BLEND_Opaque;
            In_SceneCanvas->DrawItem(TileItem);

            break;
        }
    }

    Super::Draw(In_Viewport, In_SceneCanvas);
}

bool UCustomViewport::PossesLocalPlayer(const int32 PlayerId, const int32 ControllerId)
{
    UEngine* const REF_Engine = GameInstance->GetEngine();
    const int32 NumPlayers = REF_Engine->GetNumGamePlayers(this);

    if (NumPlayers > PlayerId + 1 || ControllerId < -1)
    {
        return false;
    }

    int32 PlayerControllerId = 0;
    if (ControllerId == -1)
    {
        PlayerControllerId = UGameplayStatics::GetPlayerControllerID(REF_Engine->GetGamePlayer(this, 0)->GetPlayerController(GEngine->GetCurrentPlayWorld()));
    }

    else
    {
        PlayerControllerId = ControllerId;
    }

    REF_Engine->GetGamePlayer(this, PlayerId)->SetControllerId(PlayerControllerId);

    return true;
}

bool UCustomViewport::ChangePlayerViewSize(const int32 PlayerId, FVector2D NewRatio, FVector2D NewOrigin)
{
    UEngine* const REF_Engine = GameInstance->GetEngine();
    const int32 NumPlayers = REF_Engine->GetNumGamePlayers(this);

    if (NumPlayers > PlayerId + 1)
    {
        return false;
    }

    const TArray<ULocalPlayer*>& PlayerList = GetOuterUEngine()->GetGamePlayers(this);
    PlayerList[PlayerId]->Size = NewRatio;
    PlayerList[PlayerId]->Origin = NewOrigin;

    return true;
}

void UCustomViewport::SetBackgroundColor(FLinearColor In_Color)
{
    this->BG_Color = In_Color;
    this->BG_Type = 0;
}

void UCustomViewport::SetBackgroundTexture(UTexture2D* In_Texture)
{
    if (!IsValid(In_Texture))
    {
        return;
    }

    this->BG_Texture = In_Texture;
    this->BG_Type = 2;
}

void UCustomViewport::SetGradient(FLinearColor Start, FLinearColor End)
{
    this->GradientStart = Start;
    this->GradientEnd = End;
    this->BG_Type = 1;
}