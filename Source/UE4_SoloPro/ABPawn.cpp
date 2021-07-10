// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPawn.h"

// Sets default values
AABPawn::AABPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	RootComponent = Capsule; // 폰을 대표해서 움직임을 담당할 예정이므로 루트 컴포넌트로 지정한다.
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	Capsule->SetCapsuleHalfHeight(88.0f);
	Capsule->SetCapsuleRadius(34.0f);
	Mesh->SetRelativeLocationAndRotation(FVector(0.0f,0.0f,-88.0f),FRotator(0.0f,-90.0f,0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f,0.0f,0.0f));

	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	
	static ConstructorHelpers::FClassFinder<UAnimInstance>WARRIOR_ANIM(TEXT("/Game/Animation/WorriorAnimBlueprint.WorriorAnimBlueprint_C"));
	if(WARRIOR_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

}

// Called when the game starts or when spawned
void AABPawn::BeginPlay()
{
	Super::BeginPlay();
	/*Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr,TEXT("/Game/Animation/WarriorRun.WarriorRun"));

	if(AnimAsset != nullptr)
	{
		Mesh->PlayAnimation(AnimAsset,true);
	}*/
}

// Called every frame
void AABPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogTemp,Warning,TEXT("Pawn Create"));

}

void AABPawn::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp,Warning,TEXT("Possessed By"));
	Super::PossessedBy(NewController);
}

// Called to bind functionality to input
void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABPawn::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABPawn::LeftRight);
}

void AABPawn::UpDown(float NewAxisValue)
{
	UE_LOG(LogTemp,Warning,TEXT("%f"),NewAxisValue);
	AddMovementInput(GetActorForwardVector(),NewAxisValue); // 입력을 받아서 실제로 움직이게 해줌
}

void AABPawn::LeftRight(float NewAxisValue)
{
	UE_LOG(LogTemp,Warning,TEXT("%f"),NewAxisValue);
	AddMovementInput(GetActorRightVector(), NewAxisValue);
}

