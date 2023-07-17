// Fill out your copyright notice in the Description page of Project Settings.


#include "MonstrosityAnimInstance.h"

#include "MonstrosityCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UMonstrosityAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    MonstrosityCharacter = Cast<AMonstrosityCharacter>(TryGetPawnOwner());
}

void UMonstrosityAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (!MonstrosityCharacter)
    {
        MonstrosityCharacter = Cast<AMonstrosityCharacter>(TryGetPawnOwner());
    }
    else
    {
        FVector Velocity{ MonstrosityCharacter->GetVelocity() };
        Velocity.Z = 0.0f;
        Speed = Velocity.Size();

        bIsInAir = MonstrosityCharacter->GetCharacterMovement()->IsFalling();
        bIsAccelerating = MonstrosityCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
        bWeaponEquipped = MonstrosityCharacter->IsWeaponEquipped();
        bCrouched = MonstrosityCharacter->bIsCrouched;
        bAiming = MonstrosityCharacter->IsAiming();
    }
}
