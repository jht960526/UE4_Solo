// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		// 자기(애니메이션)의 주인(폰)에게서 데이터를 받아와서 동기화
		if(Pawn)
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if(Pawn)
	{
		FVector Speed = Pawn->GetVelocity(); // 물리,연결된 무브먼트컴포넌트가 있을때 속도 반환
		FVector LateralSpeed = FVector(Speed.X,Speed.Y,0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if(Main == nullptr)
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}
