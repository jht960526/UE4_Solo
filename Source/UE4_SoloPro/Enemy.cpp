// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));
	// 에디터상에 스켈레탈 메시에 대한 Parent Socket을 지정하지 못하는 문제해결해주기 위해서


	bOverlappingCombatSphere = false;

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController()); //컨트롤러를 가져와서 ai컨트롤러로 캐스팅

	//콜리젼 채널 ㅣ AddDynamic
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	// 생성자에 하지 않고 여기다가 해서 조금이라도 빠르게
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 애니메나 코드 영향으로 움직일 액터
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // 충돌여부와 상관없이 무시
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	// 모든 플레이어의 제어 엔티티에는 Pawn 유형이 있어야함, Pawn일때 오버렙
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if(Main)
			{
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
				if(AIController)
				{
					AIController->StopMovement();
				}
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if(Main)
			{
				Main->SetCombatTarget(this);
				CombatTarget = Main;
				bOverlappingCombatSphere = true;
				//SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking); 공격함수에 있으니까
				Attack();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if(Main)
			{
				Main->SetCombatTarget(nullptr);
				bOverlappingCombatSphere = false;
				if(EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
				{
					MoveToTarget(Main);
					CombatTarget = nullptr;
				}
			}
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if(AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f); //목표지점에 얼마나 가까워지면 성공인지

		FNavPathSharedPtr NavPath; // Enemy와 타겟의 움직인 길정보들이 저장됨

		AIController->MoveTo(MoveRequest, &NavPath); // 함수가 실행되고 나서 NavPath에 정보를 저장

		/*
		auto PathPoints = NavPath->GetPathPoints(); //TArray<FNavPathPoint>가 타입

		for(auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f,8,FLinearColor::Red, 10.f, 1.5f);
			// 어떤 오브젝트, 위치, 지름, 몇각형, 색깔, 지속시간, 두께
		}
		*/
	}
}


void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			if(Main->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if(TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				    // 어디에서, 어떤게, 위치, 회전, 자동파괴
				}
				

			}
			if(Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 충돌타입(오버랩)
	if(SwingSound)
	{
	   UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if(AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}
	if(!bAttacking)
	{
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); // 에디터에서 애니메이션 가져오기
		if(AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack"),CombatMontage); // 공격상태니까 공격모션으로
		}
		
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	if(bOverlappingCombatSphere) // 공격이 끝나더라도 오버랩되어 있으면 계속 공격
	{
		Attack();
	}
}
