// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class UE4_SOLOPRO_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "SpringArm")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UColliderMovementComponent* OurMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	FORCEINLINE UStaticMeshComponent* GetMeshComponent() {return MeshComponent;} // 스테틱메시 getter
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh;} // setter
	FORCEINLINE USphereComponent* GetSphereComponent() {return SphereComponent;} // 스피어 getter
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere;} // setter

	FORCEINLINE UCameraComponent* GetCameraComponent() {return Camera;} // 카메라 getter
	FORCEINLINE void SetCameraComponent(UCameraComponent* InCamera) { Camera = InCamera;} // setter
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() {return SpringArm;} // 스프링암 getter
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* InSpringArm) { SpringArm = InSpringArm;} // setter

private:

	void MoveForward(float input);
	void MoveRight(float input);

	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);

	FVector2D CameraInput;
};
