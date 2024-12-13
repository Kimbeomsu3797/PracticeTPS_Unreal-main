// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>

#include "AIController.h"
#include "TPSProject.h"
#include "Components/CapsuleComponent.h"
#include "EnemyAnim.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ���忡�� ATPSPlayer Ÿ�� ã�ƿ���
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer Ÿ������ ĳ����
	target = Cast<ATPSPlayer>(actor);
	// ���� ��ü ��������
	me = Cast<AEnemy>(GetOwner());

	// UEnemyAnim 할당
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());

	// AAIController 할당
	ai = Cast<AAIController>(me->GetController());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ����â�� ���� �޽��� ���
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

// ��� ����
void UEnemyFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	
	if (currentTime > idleDelayTime)
	{
		mState = EEnemyState::Move;
		currentTime = 0;

		// 애니메이션 상태 동기화
		anim->animState = mState;
		// 최초 랜덤한 위치 정해주기
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// �̵� ����
void UEnemyFSM::MoveState()
{
	// Ÿ�� ������ �ʿ�
	FVector destination = target->GetActorLocation();

	// ���� �ʿ�
	FVector dir = destination - me->GetActorLocation();

	// �������� �̵�
	//me->AddMovementInput(dir.GetSafeNormal());
	//ai->MoveToLocation((destination));

	// NavigationSystem 객체 얻어오기
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	// 목적지 길 찾기 경로 데이터 검색
	FPathFindingQuery query;
	FAIMoveRequest req;
	// 목적지에서 인지할 수 있는 범위
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(destination);
	// 길 찾기를 위한 쿼리 생성
	ai->BuildPathfindingQuery(req, query);

	if (ns != nullptr)
	{
		// 길찾기 결과 가져오기
		FPathFindingResult r = ns->FindPathSync(query);
		// 목적지까지의 길 찾기 성공 여부 확인
		if (r.Result == ENavigationQueryResult::Success)
		{
			// 타깃으로 이동
			ai->MoveToLocation(destination);
		}
		else
		{
			// 랜덤 위치로 이동
			auto result = ai->MoveToLocation(randomPos);
			// 목적지에 도착하면
			if (result == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				// 새로운 랜덤 위치 가져오기
				GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
			}
		}
	}
	
	if (dir.Size() < attackRange)
	{
		// 길 찾기 기능 정지
		ai->StopMovement();
		
		mState = EEnemyState::Attack;

		// 애니메이션 상태 동기화
		anim->animState = mState;
		// 공격 애니메이션 재생 활성화
		anim->bAttackPlay = true;
		// 공격 상태 전환 시 대기 시간이 바로 끝나도록 처리
		currentTime = attackDelayTime;
	}
}

// ���� ����
void UEnemyFSM::AttackState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > attackDelayTime)
	{
		//PRINT_LOG(TEXT("Attack!!!"));
		//��� �ð� �ʱ�ȭ
		currentTime = 0;
		anim->bAttackPlay = true;
	}

	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());

	if (distance > attackRange)
	{
		mState = EEnemyState::Move;
		// 애니메이션 상태 동기화
		anim->animState = mState;
		
		// 랜덤한 위치 정해주기
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// �ǰ� ����
void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		mState = EEnemyState::Idle;
		currentTime = 0;
		// 애니메이션 상태 동기화
		anim->animState = mState;
	}
}

// ���� ����
void UEnemyFSM::DieState()
{
	// 죽음 애니메이션 끝나기 전까지 바닥으로 안내려가게
	if (anim->bDieDone == false)
	{
		return;
	}
	
	// 계속 아래로 내려간다
	FVector P0 = me ->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;

	me->SetActorLocation(P);

	if (P.Z < -200.0f)
	{
		me->Destroy();
	}
}

void UEnemyFSM::OnDamageProcess()
{
	hp--;

	if (hp > 0)
	{
		mState = EEnemyState::Damage;

		currentTime = 0;

		// 피격 애니메이션 재생
		int32 index = FMath::RandRange(0,1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else
	{
		mState = EEnemyState::Die;

		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		anim->PlayDamageAnim(TEXT("Die"));
	}
	// 애니메이션 상태 동기화
	anim->animState = mState;
	// 길 찾기 기능 정지
	ai->StopMovement();
}

bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	return result;
}


