// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")); // 위치기능을 루트로 설정
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent()); // 카메라 루트컴포넌트에 붙임
	Camera->SetRelativeLocation(FVector(-300.f,0.f,300.f)); // 카메라 위치
	Camera->SetRelativeRotation(FRotator(-45.f,0.f,0.f)); // 카메라 각도

	//AutoPossessPlayer = EAutoReceiveInput::Player0; // 게임모드가 로컬플레이어를 자동으로 지정

	CurrentVelocity = FVector(0.f);
	MaxSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this,&ACritter::MoveForward); // 어떤 동작메르로, 누가, 실행할 함수
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),this,&ACritter::MoveRight);

}

void ACritter::MoveForward(float Value)
{
	CurrentVelocity.X = FMath::Clamp(Value,-1.f,1.f) * MaxSpeed; // Clamp: 값, 최소, 최대
}

void ACritter::MoveRight(float Value)
{
	CurrentVelocity.Y = FMath::Clamp(Value,-1.f,1.f) * MaxSpeed;
}

