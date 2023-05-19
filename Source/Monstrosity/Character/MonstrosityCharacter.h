// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MonstrosityCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class MONSTROSITY_API AMonstrosityCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMonstrosityCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    TObjectPtr<UInputMappingContext> InputMapping;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputMove;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputLook;

    TArray<TObjectPtr<UInputAction>> AllInputActions;

private:
    UPROPERTY(VisibleAnywhere, Category = Camera)
    TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, Category = Camera)
    TObjectPtr<UCameraComponent> FollowCamera;

    void MoveBackAndForward(const FInputActionValue& ActionValue);
  /*  void MoveLeftAndRight(const FInputActionValue& ActionValue);
    void TurnAtRate(const FInputActionValue& ActionValue);
    void LookUpRate(const FInputActionValue& ActionValue);*/
};
