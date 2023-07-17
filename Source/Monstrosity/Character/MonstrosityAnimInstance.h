// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "MonstrosityAnimInstance.generated.h"

class AMonstrosityCharacter;

UCLASS()
class MONSTROSITY_API UMonstrosityAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
    UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<AMonstrosityCharacter> MonstrosityCharacter;
    UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float Speed;
    UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bIsInAir{ false };
    UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bIsAccelerating{ false };
    UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bWeaponEquipped{ false };
    UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bCrouched{ false };
    UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bAiming{ false };
};
