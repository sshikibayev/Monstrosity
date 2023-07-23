// Fill out your copyright notice in the Description page of Project Settings.


#include "MonstrosityCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Monstrosity/Weapons/Weapon.h"
#include "Monstrosity/MonstrosityComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


AMonstrosityCharacter::AMonstrosityCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 600.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
    OverheadWidget->SetupAttachment(RootComponent);

    CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
    CombatComponent->SetIsReplicated(true);

    bReplicates = true;

    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void AMonstrosityCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMonstrosityCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AimOffset(DeltaTime);
}

void AMonstrosityCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (CombatComponent)
    {
        CombatComponent->Character = this;
    }
}

void AMonstrosityCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AMonstrosityCharacter, OverlappingWeapon, COND_OwnerOnly);
}

TObjectPtr<AWeapon> AMonstrosityCharacter::GetEquippedWeapon() const
{
    if (!CombatComponent)
        return nullptr;
    return CombatComponent->EquippedWeapon;
}

void AMonstrosityCharacter::SetOverlappingWeapon(TObjectPtr<AWeapon> Weapon)
{
    if (OverlappingWeapon)
    {
        OverlappingWeapon->ShowPickupWidget(false);
    }

    OverlappingWeapon = Weapon;

    if (IsLocallyControlled())
    {
        if (OverlappingWeapon)
        {
            OverlappingWeapon->ShowPickupWidget(true);
        }
    }
}

bool AMonstrosityCharacter::IsWeaponEquipped()
{
    return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool AMonstrosityCharacter::IsAiming()
{
    return (CombatComponent && CombatComponent->bAiming);
}

void AMonstrosityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PrepareInputSubsystem();
    AddingMappingContext(InputSubsystem, IMC_Character);
    BindInputActions(PlayerInputComponent);
}

void AMonstrosityCharacter::AimOffset(float DeltaTime)
{
    if (CombatComponent && !CombatComponent->EquippedWeapon)
        return;

    FVector Velocity = GetVelocity();
    Velocity.Z = 0.0f;
    float Speed = Velocity.Size();
    bool bInAir = GetCharacterMovement()->IsFalling();

    if (Speed == 0.0f && !bInAir)
    {
        FRotator CurrentAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
        FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
        AOYaw = DeltaAimRotation.Yaw;
        if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
        {
            InterpAOYaw = AOYaw;
        }
        bUseControllerRotationYaw = true;
        TurnInPlace(DeltaTime);
    }

    if (Speed > 0.0f || bInAir)
    {
        StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
        AOYaw = 0.0f;
        bUseControllerRotationYaw = true;
        TurningInPlace = ETurningInPlace::ETIP_NotTurning;
    }

    AOPitch = GetBaseAimRotation().Pitch;
    if (AOPitch > 90.0f && !IsLocallyControlled())
    {
        // Map pitch [270, 360) to [-90, 0)
        FVector2D InRange(270.0f, 360.0f);
        FVector2D OutRange(-90.0f, 0.0f);
        AOPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AOPitch);
    }
}

void AMonstrosityCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
    if (OverlappingWeapon)
    {
        OverlappingWeapon->ShowPickupWidget(true);
    }

    if (LastWeapon)
    {
        LastWeapon->ShowPickupWidget(false);
    }
}

void AMonstrosityCharacter::PrepareInputSubsystem()
{
    TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    if (!PlayerController)
        return;

    TObjectPtr<ULocalPlayer> LocalPlayer = PlayerController->GetLocalPlayer();
    if (LocalPlayer)
    {
        InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    }
}

void AMonstrosityCharacter::AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext)
{
    if (Subsystem && !MappingContext.IsNull())
    {
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(MappingContext.LoadSynchronous(), 0);
    }
}

void AMonstrosityCharacter::BindInputActions(const TObjectPtr<UInputComponent> PlayerInputComponent)
{
    TObjectPtr<UEnhancedInputComponent> PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!PlayerEnhancedInputComponent)
        return;

    if (DoInputActionsValid())
    {
        PlayerEnhancedInputComponent->BindAction(IA_Jump.LoadSynchronous(), ETriggerEvent::Started, this, &ThisClass::DoJump);
        PlayerEnhancedInputComponent->BindAction(IA_Crouch.LoadSynchronous(), ETriggerEvent::Started, this, &ThisClass::DoCrouch);
        PlayerEnhancedInputComponent->BindAction(IA_Equip.LoadSynchronous(), ETriggerEvent::Started, this, &ThisClass::Equipped);
        PlayerEnhancedInputComponent->BindAction(IA_InputMove.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::Movement);
        PlayerEnhancedInputComponent->BindAction(IA_InputLook.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::Looking);
        PlayerEnhancedInputComponent->BindAction(IA_Aim.LoadSynchronous(), ETriggerEvent::Started, this, &ThisClass::StartAim);
        PlayerEnhancedInputComponent->BindAction(IA_Aim.LoadSynchronous(), ETriggerEvent::Completed , this, &ThisClass::StopAim);
    }
}

bool AMonstrosityCharacter::DoInputActionsValid()
{
    AllInputActions.Emplace(IA_Jump);
    AllInputActions.Emplace(IA_Crouch);
    AllInputActions.Emplace(IA_InputMove);
    AllInputActions.Emplace(IA_InputLook);
    AllInputActions.Emplace(IA_Equip);

    for (auto Action : AllInputActions)
    {
        if (!Action)
        {
            UE_LOG(LogTemp, Warning, TEXT("Some input actions are not valid."));
            return false;
        }
    }

    return true;
}

void AMonstrosityCharacter::DoJump()
{
    Jump();
}

void AMonstrosityCharacter::Movement(const FInputActionValue& ActionValue)
{
    FVector2D Movement{ ActionValue.Get<FVector2D>() };

    const FVector ForwardDirection{ GetActorForwardVector() };
    const FVector RightDirection(GetActorRightVector());

    AddMovementInput(RightDirection, Movement.X);
    AddMovementInput(ForwardDirection, Movement.Y);
}

void AMonstrosityCharacter::Looking(const FInputActionValue& ActionValue)
{
    FVector2D LookAxis{ ActionValue.Get<FVector2D>() };

    AddControllerYawInput(LookAxis.X);
    AddControllerPitchInput(LookAxis.Y);
}

void AMonstrosityCharacter::Equipped()
{
    if (CombatComponent)
    {
        if (HasAuthority())
        {
            CombatComponent->EquipWeapon(OverlappingWeapon);
        }
        else
        {
            ServerEqipped();
        }
    }
}

void AMonstrosityCharacter::ServerEqipped_Implementation()
{
    if (CombatComponent)
    {
        CombatComponent->EquipWeapon(OverlappingWeapon);
    }
}

void AMonstrosityCharacter::DoCrouch()
{
    if (bIsCrouched)
    {
        UnCrouch();
    }
    else
    {
        Crouch();
    }
}

void AMonstrosityCharacter::StartAim()
{
    if (CombatComponent)
    {
        CombatComponent->SetAiming(true);
    }
}

void AMonstrosityCharacter::StopAim()
{
    if (CombatComponent)
    {
        CombatComponent->SetAiming(false);
    }
}

void AMonstrosityCharacter::TurnInPlace(float DeltaTime)
{
    if (AOYaw > 90.0f)
    {
        TurningInPlace = ETurningInPlace::ETIP_Right;
    }
    else if (AOYaw < -90.0f)
    {
        TurningInPlace = ETurningInPlace::ETIP_Left;
    }

    if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
    {
        InterpAOYaw = FMath::FInterpTo(InterpAOYaw, 0.0f, DeltaTime, 5.0f);
        AOYaw = InterpAOYaw;

        if (FMath::Abs(AOYaw) < 15.0f)
        {
            TurningInPlace = ETurningInPlace::ETIP_NotTurning;
            StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
        }
    }
}
