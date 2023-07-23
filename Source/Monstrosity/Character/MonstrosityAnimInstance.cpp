// Fill out your copyright notice in the Description page of Project Settings.


#include "MonstrosityAnimInstance.h"

#include "MonstrosityCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monstrosity/Weapons/Weapon.h"


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
        EquippedWeapon = MonstrosityCharacter->GetEquippedWeapon();
        bCrouched = MonstrosityCharacter->bIsCrouched;
        bAiming = MonstrosityCharacter->IsAiming();
        TurningInPlace = MonstrosityCharacter->GetTurningInPlace();

        FRotator AimRotation = MonstrosityCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MonstrosityCharacter->GetVelocity());
        FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
        DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 5.0f);
        YawOffset = DeltaRotation.Yaw;

        CharacterRotationLastFrame = CharacterRotation;
        CharacterRotation = MonstrosityCharacter->GetActorRotation();

        const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
        const float Target = Delta.Yaw / DeltaTime;
        const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
        Lean = FMath::Clamp(Interp , -90.0f, 90.0f);

        AOYaw = MonstrosityCharacter->GetAOYaw();
        AOPitch = MonstrosityCharacter->GetAOPitch();

        if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && MonstrosityCharacter->GetMesh())
        {
            LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
            FVector OutPosition;
            FRotator OutRotation;
            MonstrosityCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
            LeftHandTransform.SetLocation(OutPosition);
            LeftHandTransform.SetRotation(FQuat(OutRotation));
        }
    }
}
