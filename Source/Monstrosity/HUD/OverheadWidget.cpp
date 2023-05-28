// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"


void UOverheadWidget::SetDisplayText(const FText& Text)
{
    if (TB_Display)
    {
        TB_Display->SetText(Text);
    }
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
    if (InPawn)
    {
        ENetRole LocalRole{ InPawn->GetLocalRole() };
        FText Role;
        switch (LocalRole)
        {
            case ENetRole::ROLE_Authority:
                Role = FText::FromString("Authority");
                break;
            case ENetRole::ROLE_AutonomousProxy:
                Role = FText::FromString("Autonomous Proxy");
                break;
            case ENetRole::ROLE_SimulatedProxy:
                Role = FText::FromString("Simulated Proxy");
                break;
            case ENetRole::ROLE_None:
                Role = FText::FromString("None");
                break;
        }

        FText MessageAboutRole = FText::FromString(FString::Printf(TEXT("Local Role: %s"), *Role.ToString()));
        SetDisplayText(MessageAboutRole);
        if (InPawn->GetPlayerState(); TObjectPtr<APlayerState> PlayerState = InPawn->GetPlayerState())
        {
            SetDisplayText(FText::FromString(PlayerState->GetPlayerName()));
        }
    }
}

void UOverheadWidget::NativeDestruct()
{

    Super::NativeDestruct();
}
