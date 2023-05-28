// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

class UTextBlock;

UCLASS()
class MONSTROSITY_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void SetDisplayText(const FText& Text);

    UFUNCTION(BlueprintCallable)
    void ShowPlayerNetRole(APawn* InPawn);
    //void ShowPlayerNetRole(TObjectPtr<APawn> InPawn);

protected:
    virtual void NativeDestruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TB_Display;
};
