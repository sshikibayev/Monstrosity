// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class AMonstrosityCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MONSTROSITY_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    friend AMonstrosityCharacter;

    void EquipWeapon(const TObjectPtr<AWeapon> WeaponToEquip);

protected:
	virtual void BeginPlay() override;

private:
    TObjectPtr<AMonstrosityCharacter> Character;
    TObjectPtr<AWeapon> EquippedWeapon;
};
