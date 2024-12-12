// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerFire.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerFire : public UPlayerBaseComponent
{
	GENERATED_BODY()
	UPlayerFire();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;
public:
	//�߻� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Fire;
	//�Ѿ� ���� (�����ջ��� ����)
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;
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
	// �Ѿ� ���� ȿ�� ����
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

	// �Ϲ� ���� ũ�ν����
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;
	// ũ�ν���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	// 카메리 셰이크 블루프린트를 저장할 변수
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;
	//ī�޶� Ŭ����
	UPROPERTY()
	class UCameraComponent* tpsCamComp;
	//�� ���̷�Ż�޽� Ŭ����
	UPROPERTY()
	class USkeletalMeshComponent* gunMeshComp;
	//�������� ����ƽ �޽� Ŭ����
	UPROPERTY()
	class UStaticMeshComponent* sniperGunComp;
};
