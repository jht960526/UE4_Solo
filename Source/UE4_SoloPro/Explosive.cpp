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

	if(OtherActor) //������ ���� ���Ͷ��
	{
		AMain* Main = Cast<AMain>(OtherActor); // ������ ���͸� Main���� ��ȯ��Ų��.
		if(Main)
		{
			if(OverlapParticles)
	        {
		        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(),FRotator(0.f),true);
		        // ������ ��ġ���� ��ƼŬ ���,ȸ��,�߻�    // ����, ��ƼŬ�ý���, Ʈ������(�����̼�), (ȸ��)�����̼�, �ڵ��ı� ��
	        }

	        if(OverlapSound)
	        {
		        UGameplayStatics::PlaySound2D(this, OverlapSound);
	        }
			Main->DecrementHealth(Damage);

			Destroy(); // ���Ⱑ ������� �ȵǴϱ� ���� ������ֱ�
		}
	}
}


void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);
}

