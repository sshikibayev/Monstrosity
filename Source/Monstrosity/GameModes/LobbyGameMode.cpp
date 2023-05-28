// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 NumberOfPlayer{ GameState.Get()->PlayerArray.Num() };

    if (NumberOfPlayer == 2)
    {
        bUseSeamlessTravel = true;
        GetWorld()->ServerTravel(FString("/Game/Levels/Main?listen"));
    }
}