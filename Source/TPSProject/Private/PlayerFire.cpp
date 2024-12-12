// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFire.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include "EnemyFSM.h"
#include <Camera/CameraComponent.h>
#include "PlayerAnim.h"


UPlayerFire::UPlayerFire()
{
	// tick 호출되도록 처리
	PrimaryComponentTick.bCanEverTick = true;
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
	
}

void UPlayerFire::SetupInputBinding(class UEnhancedInputComponent* PlayerInput)
{
	//�߻� �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &UPlayerFire::InputFire);
		

	//�� �� ��ü �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &UPlayerFire::ChangeToGrenadeGun);
	PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &UPlayerFire::ChangeToSniperGun);

	// �������� ���� ��� �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Started, this, &UPlayerFire::SniperAim);
	PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Completed, this, &UPlayerFire::SniperAim);
}


void UPlayerFire::BeginPlay()
{
	Super::BeginPlay();
	tpsCamComp = me->tpsCamComp;
	gunMeshComp = me->gunMeshComp;
	sniperGunComp = me->sniperGunComp;
	
	// �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	_crosshairUI->AddToViewport();
	//�⺻������ �������۸� �����ϵ��� ����
	ChangeToSniperGun(FInputActionValue());
}

void UPlayerFire::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
}

//�߻� �Լ�
void UPlayerFire::InputFire(const FInputActionValue& inputValue)
{
	// 발사 소리 재생
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);
	
	// 카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
	
	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();
	
	// ��ź�� ����
	if (bUsingGrenadeGun)
	{
		//�Ѿ� �߻� ó��
		FTransform firePosition = me->gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else // ���� ����
	{
		// LineTrace�� ���� ��ġ
		FVector startPos = tpsCamComp->GetComponentLocation();
		// LineTrace�� ���� ��ġ
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// LineTrace�� �浹 ������ ���� ����
		FHitResult hitinfo;
		// �浹 �ɼ� ���� ����
		FCollisionQueryParams params;
		// �ڱ� �ڽ��� �浹���� ����
		params.AddIgnoredActor(me);

		// Channel ���͸� �̿��� LineTrace �浹 ����
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitinfo, startPos, endPos, ECC_Visibility, params);
		// LineTrace�� �ε����� ��
		if (bHit)
		{
			// �浹 ó�� -> �Ѿ� ���� ȿ�� ���
			FTransform bulletTrans;
			// �ε��� ��ġ �Ҵ�
			bulletTrans.SetLocation(hitinfo.ImpactPoint);
			// �Ѿ� ���� ȿ�� �ν��Ͻ� ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitinfo.GetComponent();
			// ���� ������Ʈ�� ������ ����Ǿ� �ִٸ�
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// ������ ������ �ʿ�
				FVector dir = (endPos - startPos).GetSafeNormal();
				// �������� ��
				FVector force = dir * hitComp->GetMass() * 500000;
				// �� �������� ������
				hitComp->AddForceAtLocation(force, hitinfo.ImpactPoint);
			}

			auto enemy = hitinfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
}
//�⺻ �� ȣ�� �Լ�
void UPlayerFire::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

//�������� �� ȣ�� �Լ�
void UPlayerFire::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

// ���� ����
void UPlayerFire::SniperAim(const struct FInputActionValue& inputValue)
{
	// �������۰� �ƴ϶�� ó��x
	if (bUsingGrenadeGun)
	{
		return;
	}

	// Pressed �Է� ó��
	if (bSniperAim == false)
	{
		// ���� ���� Ȱ��ȭ
		bSniperAim = true;
		// �������� UI ���
		_sniperUI->AddToViewport();
		// ī�޶��� �þ߰� fov ����
		tpsCamComp->SetFieldOfView(45.0f);
		// �Ϲ� ���� ui ����
		_crosshairUI->RemoveFromParent();
	}
	else
	{
		// ���� ���� ��� ��Ȱ��ȭ
		bSniperAim = false;
		// ���� ���� UI ȭ�鿡�� ����
		_sniperUI->RemoveFromParent();
		// ī�޶� �þ߰� ����
		tpsCamComp->SetFieldOfView(90.0f);
		// �Ϲ� ���� UI ���
		_crosshairUI->AddToViewport();
	}
}