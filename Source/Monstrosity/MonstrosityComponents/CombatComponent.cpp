// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Monstrosity/Weapons/Weapon.h"
#include "Monstrosity/Character/MonstrosityCharacter.h"
#include "Engine/SkeletalMeshSocket.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
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
        EquippedWeapon->ShowPickupWidget(false);
    }
}