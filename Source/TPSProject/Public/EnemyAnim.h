// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 상태 머신 기억 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EEnemyState animState;
	// 공격 상태 재생할 지 여부
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = FSM)
	bool bAttackPlay = false;

	// 공격 애니메이션
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
	void OnEndAttackAnimation();

	// 피격 애니메이션 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayDamageAnim(FName sectionName);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	bool bDieDone = false;
};
