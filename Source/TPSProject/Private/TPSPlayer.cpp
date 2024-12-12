// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include <Components/StaticMeshComponent.h>
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerAnim.h"
#include "PlayerMove.h"

#include "EnemyFSM.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1.���̷�Ż�޽� �����͸� �ҷ�����ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh
	(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		//2. Mesh ������Ʈ�� ��ġ�� �����ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	//3.TPS ī�޶� ���̰� �ʹ�.
	//3-1. SpringArm������Ʈ ���̱�
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringAtmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	//�ش� ��Ʈ�ѷ� �����̼��� Ʈ���
	springArmComp->bUsePawnControlRotation = true;
	//3-2. Camera������Ʈ�� ���δ�.

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	//�ش� ��Ʈ�ѷ��� Yaw�� Ʈ��� ���� Yaw�� z�� ȸ�� Roll�� X�� ȸ�� Pich�� Y�� ȸ��
	tpsCamComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;
	JumpMaxCount = 2;

	// 4. �� ���̷�Ż�޽� ������Ʈ ���
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// 4-2. ���̷�Ż�޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if(TempGunMesh.Succeeded())
	{
		//4-4 ���̷�Ż �޽� ������ �Ҵ�
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		//4-5 ��ġ �����ϱ�
		gunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
		gunMeshComp->SetRelativeRotation(FRotator(0,90,0));
	}

	//5. �������۰� ������Ʈ ���
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	//5-1. �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	//5-2. �ش� ������Ʈ�� ��θ� �ε����ش�
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	//5-3. �ε忡 �����ߴٸ�
	if (TempSniperMesh.Succeeded())
	{
		//5-4. ����ƽ�޽� ������ �Ҵ�
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		//5-5. ��ġ ����
		sniperGunComp->SetRelativeLocation(FVector(-42, 7, 1));
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));
		//5-6 ũ�� �����ϱ�
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
	// �⺻���� ��������
	ChangeToSniperGun(FInputActionValue());
	
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
	playerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("PlayerMove"));
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	//�⺻������ �������۸� �����ϵ��� ����
	ChangeToSniperGun(FInputActionValue());

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}

	// �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	_crosshairUI->AddToViewport();
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//��ǲ ���ε� �κ�
// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput)
	{
		// 컴포넌트에서 입력 바인딩 처리하도록 호출
		playerMove->SetupInputBinding(PlayerInput);

		//�߻� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);

		//�� �� ��ü �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);

		// �������� ���� ��� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);
	}
}

// ���� ����
void ATPSPlayer::SniperAim(const struct FInputActionValue& inputValue)
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

//�߻� �Լ�
void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{
	// 발사 소리 재생
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);
	
	// 카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
	
	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();
	
	// ��ź�� ����
	if (bUsingGrenadeGun)
	{
		//�Ѿ� �߻� ó��
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
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
		params.AddIgnoredActor(this);

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
void ATPSPlayer::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

//�������� �� ȣ�� �Լ�
void ATPSPlayer::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}