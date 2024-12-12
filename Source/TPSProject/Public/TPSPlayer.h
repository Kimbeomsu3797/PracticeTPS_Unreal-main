// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInputBindingDelegate, class UEnhancedInputComponent*);
UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	//입력 바인딩 델리게이트
	FInputBindingDelegate onInputBindingDelegate;
	// Sets default values for this character's properties
	ATPSPlayer();

	//��ǲ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputMappingContext* imc_TPS;

	
	
	

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
	

	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerBaseComponent* playerMove;

	//UPROPERTY(VisibleAnywhere, Category = Component)
	//class UPlayerBaseComponent* playerFire;
};
