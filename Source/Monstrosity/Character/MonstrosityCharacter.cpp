// Fill out your copyright notice in the Description page of Project Settings.


#include "MonstrosityCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

//#include "../../../../../../UE_5.1/Engine/Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"

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

    APlayerController* PlayerController = Cast<APlayerController>(GetController());

    if (!PlayerController)
        return;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
        Subsystem->ClearAllMappings();

        if (!InputMapping)
            return;
        Subsystem->AddMappingContext(InputMapping, 0);
    }

    UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!PlayerEnhancedInputComponent)
        return;

    AllInputActions.Emplace(InputMove);
    AllInputActions.Emplace(InputLook);

    bool bIsActionsValid{ true };
    for (auto Action : AllInputActions)
    {
        if (!Action)
        {
            UE_LOG(LogTemp, Warning, TEXT("The %s action is not valid"), *Action->GetName());
            bIsActionsValid =  false;
        }
    }

    if (bIsActionsValid)
    {
        PlayerEnhancedInputComponent->BindAction(InputMove, ETriggerEvent::Started, this, &ThisClass::MoveBackAndForward);
    }
}

void AMonstrosityCharacter::MoveBackAndForward(const FInputActionValue& ActionValue)
{
     // Debug log output to confirm that the handler function is running.
    //UE_LOG(LogTemp, Warning, TEXT("%s called with Input Action Value %s (magnitude %f)"), *ActionValue.ToString(), ActionValue.GetMagnitude());
    UE_LOG(LogTemp, Warning, TEXT("MoveBackAndForward"));

    // Use the GetType() function to determine Value's type, and the [] operator with an index between 0 and 2 to access its data.
}

//void AMonstrosityCharacter::MoveLeftAndRight(const FInputActionValue& ActionValue)
//{
//    //UE_LOG(LogTemp, Warning, TEXT("MoveLeftAndRight: %i"), (int)ActionValue.GetValueType());
//}
//
//void AMonstrosityCharacter::TurnAtRate(const FInputActionValue& ActionValue)
//{
//}
//
//void AMonstrosityCharacter::LookUpRate(const FInputActionValue& ActionValue)
//{
//    //UE_LOG(LogTemp, Warning, TEXT("LookUp: %f"), (float)ActionValue.GetValueType());
//}
