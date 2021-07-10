// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UE4_SoloPro.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE4_SOLOPRO_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override; // ��Ʈ�ѷ� ���� ���� Ȯ��
	virtual void OnPossess(APawn* aPawn) override; // �������� ���� Ȯ�� 4.22���� ���� Possess�� Final�� �����Ǿ� ���̻� ��� x
	
protected:

	virtual void BeginPlay() override;
};
