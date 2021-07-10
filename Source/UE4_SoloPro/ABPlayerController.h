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
	virtual void PostInitializeComponents() override; // 컨트롤러 생성 시점 확인
	virtual void OnPossess(APawn* aPawn) override; // 빙의진행 시점 확인 4.22버젼 이후 Possess는 Final로 지정되어 더이상 상속 x
	
protected:

	virtual void BeginPlay() override;
};
