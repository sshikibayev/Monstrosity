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
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void SetAiming(bool bNewAiming);
    UFUNCTION(Server, Reliable)
    void ServerSetAiming(bool bNewAiming);

private:
    TObjectPtr<AMonstrosityCharacter> Character;
    UPROPERTY(Replicated)
    TObjectPtr<AWeapon> EquippedWeapon;

    UPROPERTY(Replicated)
    bool bAiming;
};
