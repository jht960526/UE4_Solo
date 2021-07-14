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
	// ����ƽ �޽ñ�� �߰�, class�� ���漱�� �����̶� �����̴� �� �Ұ���
	// ���̷��� �޽ô� �ִϸ��̼ǰ��� �����̴� �� �߰� ����

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera; // ī�޶� ��� ����

	UPROPERTY(EditAnywhere,Category = "Pawn Movement")
	float MaxSpeed; // ���������� �ٲ� �� �ְ�

private:

	void MoveForward(float Value);
	void MoveRight(float Value);

	FVector CurrentVelocity;
	

};
