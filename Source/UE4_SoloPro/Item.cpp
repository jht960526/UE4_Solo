// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));

	RootComponent = CollisionVolume;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	IdleParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdelParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	bRotate = false;
	RotationRate = 45.f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this,&AItem::OnOverlapBegin); // 무언가 겹치기 시작할때
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this,&AItem::OnOverlapEnd);

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bRotate)
	{
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += DeltaTime * RotationRate;
		SetActorRotation(Rotation);
	}
}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning,TEXT("Super::OnOverlapBegin()"));

	if(OverlapParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(),FRotator(0.f),true);
		// 지정된 위치에서 파티클 재생,회전,발사    // 월드, 파티클시스템, 트렌스폼(로케이션), (회전)로테이션, 자동파괴 불
	}
	Destroy();

	if(OverlapSound)
	{
		UGameplayStatics::PlaySound2D(this,OverlapSound);
	}
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp,Warning,TEXT("Super::OnOverlapEnd()"));
}

