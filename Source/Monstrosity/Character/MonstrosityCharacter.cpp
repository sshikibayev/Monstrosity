// Fill out your copyright notice in the Description page of Project Settings.


#include "MonstrosityCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"


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
}

void AMonstrosityCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMonstrosityCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMonstrosityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PrepareInputSubsystem();
    AddingMappingContext(InputSubsystem, IM_Character);
    BindInputActions(PlayerInputComponent);
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
        PlayerEnhancedInputComponent->BindAction(IA_InputMove.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::Movement);
        PlayerEnhancedInputComponent->BindAction(IA_InputLook.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::Looking);
    }
}

bool AMonstrosityCharacter::DoInputActionsValid()
{
    AllInputActions.Add(IA_InputMove);
    AllInputActions.Add(IA_InputLook);

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