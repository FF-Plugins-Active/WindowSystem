// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomViewport.h"

UCustomViewport::UCustomViewport() : Super(FObjectInitializer::Get())
{

}

void UCustomViewport::UpdateActiveSplitscreenType()
{
    Super::UpdateActiveSplitscreenType();
}

void UCustomViewport::LayoutPlayers()
{
    UpdateActiveSplitscreenType();
    const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();

    // Initialize the players
    const TArray<ULocalPlayer*>& PlayerList = GetOuterUEngine()->GetGamePlayers(this);

    if (PlayerList.Num() == 1)
    {
        PlayerList[0]->Size.X = 0.75;
        PlayerList[0]->Size.Y = 0.75;

        PlayerList[0]->Origin.X = SplitscreenInfo[SplitType].PlayerData[0].OriginX;
        PlayerList[0]->Origin.Y = SplitscreenInfo[SplitType].PlayerData[0].OriginY;

        return;
    }

    else if (PlayerList.Num() == 3)
    {
        PlayerList[0]->Size.X = 0.425;
        PlayerList[0]->Size.Y = 0.9;
        PlayerList[0]->Origin.X = 0.525;
        PlayerList[0]->Origin.Y = 0.05;

        PlayerList[1]->Size.X = 0.425;
        PlayerList[1]->Size.Y = 0.425;
        PlayerList[1]->Origin.X = 0.05;
        PlayerList[1]->Origin.Y = 0.05;

        PlayerList[2]->Size.X = 0.425;
        PlayerList[2]->Size.Y = 0.425;
        PlayerList[2]->Origin.X = 0.05;
        PlayerList[2]->Origin.Y = 0.525;

        return;
    }

    for (int32 PlayerIdx = 0; PlayerIdx < PlayerList.Num(); PlayerIdx++)
    {
        if (SplitType < SplitscreenInfo.Num() && PlayerIdx < SplitscreenInfo[SplitType].PlayerData.Num())
        {
            PlayerList[PlayerIdx]->Size.X = 0.5;
            PlayerList[PlayerIdx]->Size.Y = 0.5;
            
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