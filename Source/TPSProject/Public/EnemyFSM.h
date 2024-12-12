// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// ����� ���� ����
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;

	// ��� ����
	void IdleState();

	// �̵� ����
	void MoveState();

	// ���� ����
	void AttackState();

	// �ǰ� ����
	void DamageState();

	// ���� ����
	void DieState();

	// ���ð�
	UPROPERTY(EditDefaultsOnly, Category = FSM)
		float idleDelayTime = 2;

	// ��� �ð�
	float currentTime = 0;

	// Ÿ��
	UPROPERTY(VisibleAnywhere, Category = FSM)
		class ATPSPlayer* target;

	// ���� ����
	UPROPERTY()
		class AEnemy* me;

	// ���� ����
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 150.0f;

	// ���� ��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 2.0f;

	// 피격 알림 이벤트 함수
	void OnDamageProcess();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 hp = 3;

	UPROPERTY(EditAnywhere, category = FSM)
	float damageDelayTime = 2.0f;

	UPROPERTY(EditAnywhere, category = FSM)
	float dieSpeed = 50.0f;

	// 사용중인 애니메이션 블루프린트
	UPROPERTY()
	class UEnemyAnim* anim;

	// Enemy를 소유하고 있는 AIController
	UPROPERTY()
	class AAIController* ai;

	// 길 찾기 수행시 랜덤 위치
	FVector randomPos;
	// 랜덤 위치 가져오기
	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest); 
};
