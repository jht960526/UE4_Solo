// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UE4_SoloPro.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE4_SOLOPRO_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AABGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override; // 플레이어가 조종할 폰을 생성하고 플레이어 컨트롤러가 빙의하는 작업
};
