// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UCLASS()
class UE4_SOLOPRO_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	//전방 선언 하는 이유: 보통 include는 헤더 파일에서 하지만 더 빠르게 사용하기 위해 cpp파일에 해주고 헤더파일에서 전방선언해줌
	/* Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) // AllowPrivateAccess는 C++를 블프에서 사용가능하게 해줌
	class USpringArmComponent* CameraBoom;

	/* Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* Base turn rates to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Camera)
	float BaseLookUpRate;

	/**
	/*
	/* Player Stats
	/*
	*/

	//UPROPERTY(EditDefaultsOnly, VisibleAnywhere, Category = "Player Stats")
	// 디폴트창에서만 수정가능하다고 했으면서 어디서든 볼수 있다고 체크해서 문제가 생김

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins; // int가 어떤 플랫폼에서든 오류 없게 사용하기 위해서

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Called for forwards/backwards input */
	void MoveForward(float Value);

	/* Called for side to side input */
	void MoveRight(float Value);

	/* Called via input to turn at a given rate
	   @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/* Called via input to look up/ down at a given rate
	   @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/ down rate
	*/
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const {return CameraBoom;}
	FORCEINLINE class UCameraComponent* GetFollowCamera() const {return FollowCamera;}

};
