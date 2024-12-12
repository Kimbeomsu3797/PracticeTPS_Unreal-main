// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

	//��ǲ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputMappingContext* imc_TPS;

	//�߻� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Fire;
	//�߻� �Լ�
	void InputFire(const struct FInputActionValue& inputValue);

	//�� �� ��ü Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_GrenadeGun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_SniperGun;

	//��ź���� ����ϰ� �ִ����� ���� ����
	bool bUsingGrenadeGun = true;

	//�� ���� ��ü �Լ�
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);

	void ChangeToSniperGun(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Sniper;

	// �������� ���� ó�� �Լ�
	void SniperAim(const struct FInputActionValue& inputValue);

	// �������� ���� ������ ����
	bool bSniperAim = false;

	// �������� UI ���� ����
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
		TSubclassOf<class UUserWidget> sniperUIFactory;
	
	// �������� UI ���� �ν��Ͻ�
	UPROPERTY()
		class UUserWidget* _sniperUI;

	//�Ѿ� ���� (�����ջ��� ����)
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
		TSubclassOf<class ABullet> bulletFactory;

	// �Ѿ� ���� ȿ�� ����
	UPROPERTY(EditAnywhere, Category = BulletEffect)
		class UParticleSystem* bulletEffectFactory;

	// �Ϲ� ���� ũ�ν����
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
		TSubclassOf<class UUserWidget> crosshairUIFactory;
	// ũ�ν���� �ν��Ͻ�
	UPROPERTY()
		class UUserWidget* _crosshairUI;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//�������� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* springArmComp;
	//ī�޶� Ŭ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* tpsCamComp;
	//�� ���̷�Ż�޽� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
		class USkeletalMeshComponent* gunMeshComp;
	//�������� ����ƽ �޽� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
		class UStaticMeshComponent* sniperGunComp;

	// 카메리 셰이크 블루프린트를 저장할 변수
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;

	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerBaseComponent* playerMove;
};
