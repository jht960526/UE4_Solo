// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

AExplosive::AExplosive()
{
	Damage = 15.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);

	if(OtherActor) //접촉한 것이 액터라면
	{
		AMain* Main = Cast<AMain>(OtherActor); // 접촉한 액터를 Main으로 변환시킨다.
		if(Main)
		{
			if(OverlapParticles)
	        {
		        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(),FRotator(0.f),true);
		        // 지정된 위치에서 파티클 재생,회전,발사    // 월드, 파티클시스템, 트렌스폼(로케이션), (회전)로테이션, 자동파괴 불
	        }

	        if(OverlapSound)
	        {
		        UGameplayStatics::PlaySound2D(this, OverlapSound);
	        }
			Main->DecrementHealth(Damage);

			Destroy(); // 무기가 사라지면 안되니까 따로 만들어주기
		}
	}
}


void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);
}

