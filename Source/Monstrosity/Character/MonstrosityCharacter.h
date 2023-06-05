// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MonstrosityCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;

UCLASS()
class MONSTROSITY_API AMonstrosityCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMonstrosityCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void PostInitializeComponents() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    FORCEINLINE TObjectPtr<UEnhancedInputLocalPlayerSubsystem> GetInputSubsystem(){ return InputSubsystem;}

    void SetOverlappingWeapon(TObjectPtr<AWeapon> Weapon);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Controls|Input Mappings")
    TSoftObjectPtr<UInputMappingContext> IM_Character;
    UPROPERTY(EditDefaultsOnly, Category = "Controls|Input Actions")
    TSoftObjectPtr<UInputAction> IA_Jump;
    UPROPERTY(EditDefaultsOnly, Category = "Controls|Input Actions")
    TSoftObjectPtr<UInputAction> IA_Equip;
    UPROPERTY(EditDefaultsOnly, Category = "Controls|Input Actions")
    TSoftObjectPtr<UInputAction> IA_InputMove;
    UPROPERTY(EditDefaultsOnly, Category = "Controls|Input Actions")
    TSoftObjectPtr<UInputAction> IA_InputLook;

    TArray<TSoftObjectPtr<UInputAction>> AllInputActions;

private:
    UPROPERTY(VisibleAnywhere, Category = Camera)
    TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, Category = Camera)
    TObjectPtr<UCameraComponent> FollowCamera;
    TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UWidgetComponent> OverheadWidget;
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
    TObjectPtr<AWeapon> OverlappingWeapon;
    UFUNCTION()
    void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

    void PrepareInputSubsystem();
    void AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext);
    void BindInputActions(const TObjectPtr<UInputComponent> PlayerInputComponent);
    bool DoInputActionsValid();
    void DoJump();
    void Equipped();
    void Movement(const struct FInputActionValue& ActionValue);
    void Looking(const struct FInputActionValue& ActionValue);
};