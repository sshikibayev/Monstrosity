// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Monstrosity/Weapons/Weapon.h"
#include "Monstrosity/Character/MonstrosityCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
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
}

void UCombatComponent::ServerSetAiming_Implementation(bool bNewAiming)
{
    bAiming = bNewAiming;

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
    }
}