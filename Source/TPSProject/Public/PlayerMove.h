// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMove.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerMove : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	UPlayerMove();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;
	// 좌우 회전
	void Turn(const struct FInputActionValue& inputValue);
	// 상하 회전
	void LookUp(const struct FInputActionValue& inputValue);

	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;
	FVector direction;
	
	void Move(const struct FInputActionValue& inputValue);
	
	// 이동 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 200;
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;
	
	// 플레이어 이동 처리
	void PlayerMove();

	// 달리기 입력
	UPROPERTY(EditDefaultsOnly, category = "Input")
	class UInputAction* ia_Run;
	// 달리기 이벤트 처리 함수
	void InputRun();

	// 점프 입력
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;
	// 점프
	void InputJump(const struct FInputActionValue& inputValue);
};
