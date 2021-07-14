// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Critter.generated.h"

UCLASS()
class UE4_SOLOPRO_API ACritter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACritter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* MeshComponent; 
	// 스태틱 메시기능 추가, class는 전방선언 정적이라 움직이는 것 불가능
	// 스켈레톤 메시는 애니메이션같이 움직이는 것 추가 가능

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera; // 카메라 기능 구현

	UPROPERTY(EditAnywhere,Category = "Pawn Movement")
	float MaxSpeed; // 블프에서도 바꿀 수 있게

private:

	void MoveForward(float Value);
	void MoveRight(float Value);

	FVector CurrentVelocity;
	

};
