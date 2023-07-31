// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;

UCLASS()
class MONSTROSITY_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
    AProjectile();
    virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
    TObjectPtr<UBoxComponent> CollisionBox;
};
