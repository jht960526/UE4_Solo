// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if(Pawn)
		{
			Enemy = Cast<AEnemy>(Pawn);
		}
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if(Pawn)
		{
			FVector Speed = Pawn->GetVelocity(); // 물리,연결된 무브먼트컴포넌트가 있을때 속도 반환
		    FVector LateralSpeed = FVector(Speed.X,Speed.Y,0.f);
		    MovementSpeed = LateralSpeed.Size();

			Enemy = Cast<AEnemy>(Pawn);
		}
	}
}
