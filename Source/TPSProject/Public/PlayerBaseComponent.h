// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerBaseComponent();
	virtual void InitializeComponent() override;
protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) {};
	UPROPERTY()
	ATPSPlayer* me;
	UPROPERTY()
	UCharacterMovementComponent* moveComp;
};
