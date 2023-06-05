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
}

void AMonstrosityCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMonstrosityCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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

void AMonstrosityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PrepareInputSubsystem();
    AddingMappingContext(InputSubsystem, IM_Character);
    BindInputActions(PlayerInputComponent);
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
        PlayerEnhancedInputComponent->BindAction(IA_Equip.LoadSynchronous(), ETriggerEvent::Started, this, &ThisClass::Equipped);
        PlayerEnhancedInputComponent->BindAction(IA_InputMove.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::Movement);
        PlayerEnhancedInputComponent->BindAction(IA_InputLook.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::Looking);
    }
}

bool AMonstrosityCharacter::DoInputActionsValid()
{
    AllInputActions.Emplace(IA_Jump);
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
    if (HasAuthority() && CombatComponent)
    {
        CombatComponent->EquipWeapon(OverlappingWeapon);
    }
}
