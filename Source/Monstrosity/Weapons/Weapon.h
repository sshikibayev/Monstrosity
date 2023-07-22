// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    EWS_Initial UMETA(DisplayName = "Initial State"),
    EWS_Equipped UMETA(DisplayName = "Equipped"),
    EWS_Dropped UMETA(DisplayName = "Dropped"),

    EWS_MAX UMETA(DisplayName = "Default MAX")
};

class USphereComponent;
class UWidgetComponent;

UCLASS()
class MONSTROSITY_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
    virtual void Tick(float DeltaTime) override;

    void ShowPickupWidget(bool bShowWidget);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    FORCEINLINE TObjectPtr<USphereComponent> GetAreaSphere() const { return AreaSphere; }
    FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetWeaponMesh() const { return WeaponMesh; }

    void SetWeaponState(EWeaponState NewState);

protected:
	virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnSphereOverlap
    (
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    virtual void OnSphereEndOverlap
    (
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex
    );

private:

    UFUNCTION()
    void OnRep_WeaponState();

    UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
    TObjectPtr<USkeletalMeshComponent> WeaponMesh;

    UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
    TObjectPtr<USphereComponent> AreaSphere;

    UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleANywhere, Category = "Weapon Properties")
    EWeaponState WeaponState;

    UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
    TObjectPtr<UWidgetComponent> PickupWidget;
};
