// Fill out your copyright notice in the Description page of Project Settings.

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "PlayerMove.h"

UPlayerMove::UPlayerMove()
{
	// tick 호출되도록 처리
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();
	
	// 초기 속도를 걷기로 설정
	me->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}

void UPlayerMove::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PlayerMove();
}

//�¿� ȸ�� �Լ�
void UPlayerMove::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerYawInput(value);
}

//���Ʒ� ȸ�� �Լ�
void UPlayerMove::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerPitchInput(value);
}

void UPlayerMove::SetupInputBinding(class UEnhancedInputComponent* PlayerInput)
{
	PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &UPlayerMove::Turn);
	PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &UPlayerMove::LookUp);
	PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &UPlayerMove::Move);
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &UPlayerMove::InputRun);
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &UPlayerMove::InputRun);
	PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &UPlayerMove::InputJump);
}

//�̵� ó�� �Լ�
void UPlayerMove::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	// 상하 입력 이벤트 처리
	direction.X = value.X;
	// 좌우 입력 이벤트 처리
	direction.Y = value.Y;
}

void UPlayerMove::PlayerMove()
{
	//�÷��̾� �̵�ó��
	//��� �
	//P(��� ��ġ) = P0(���� ��ġ) + V(�ӵ�) X T(�ð�)
	//direction = FTransform(GetControlRotation()).TransformVector(direction);
	//FVector P0 = GetActorLocation();
	//FVector vt = direction * walkSpeed * DeltaTime;
	//FVector P = P0 + vt;
	//SetActorLocation(P);
	//�ش� ������ ���� ���ִ� �Լ��� ���������
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void UPlayerMove::InputRun()
{
	auto movement = me->GetCharacterMovement();

	if (movement->MaxWalkSpeed > walkSpeed)
	{
		movement->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

//���� �Լ�
void UPlayerMove::InputJump(const FInputActionValue& inputValue)
{
	me->Jump();
}
