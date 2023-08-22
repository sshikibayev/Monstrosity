// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Monstrosity/Weapons/Weapon.h"
#include "Monstrosity/Character/MonstrosityCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"


UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    BaseWalkSpeed = 600.0f;
    AimWalkSpeed = 400.0f;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    if (Character)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
    }
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCombatComponent, EquippedWeapon);
    DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::SetAiming(bool bNewAiming)
{
    bAiming = bNewAiming;
    ServerSetAiming(bNewAiming);
    if (Character)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
    }
}

void UCombatComponent::ServerSetAiming_Implementation(bool bNewAiming)
{
    bAiming = bNewAiming;
    if (Character)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
    }
}

void UCombatComponent::ToggleFire(bool bStarted)
{
    bFireStarted = bStarted;

    if (bFireStarted)
    {
        ServerFire();
    }
}

void UCombatComponent::ServerFire_Implementation()
{
    MulticastFire();
}

void UCombatComponent::MulticastFire_Implementation()
{
    if (EquippedWeapon == nullptr)
        return;

    if (Character)
    {
        Character->PlayFireMontage(bAiming);
        EquippedWeapon->Fire();
    }
}

void UCombatComponent::OnRep_EquippedWeapon()
{
    if (EquippedWeapon && Character)
    {
        Character->GetCharacterMovement()->bOrientRotationToMovement = false;
        Character->bUseControllerRotationYaw = true;
    }
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::EquipWeapon(const TObjectPtr<AWeapon> WeaponToEquip)
{
    if (Character && WeaponToEquip)
    {
        EquippedWeapon = WeaponToEquip;
        EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
        const auto HandSocket{ Character->GetMesh()->GetSocketByName(FName("RightHandSocket")) };

        if (HandSocket)
        {
            HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
        }
        EquippedWeapon->SetOwner(Character);
        Character->GetCharacterMovement()->bOrientRotationToMovement = false;
        Character->bUseControllerRotationYaw = true;
    }
}