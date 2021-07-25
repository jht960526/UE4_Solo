// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Main.h"

APickup::APickup()
{
	CoinCount = 1;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);

	UE_LOG(LogTemp,Warning,TEXT("Pickup::OnOverlapBegin()"));

	if(OtherActor) //������ ���� ���Ͷ��
	{
		AMain* Main = Cast<AMain>(OtherActor); // ������ ���͸� Main���� ��ȯ��Ų��.
		if(Main)
		{
			Main->IncrementCoins(CoinCount);

			Destroy(); // ���Ⱑ ������� �ȵǴϱ� ���� ������ֱ�
		}
	}
}


void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);

	UE_LOG(LogTemp,Warning,TEXT("Pickup::OnOverlapBegin()"));
}