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

	if(OtherActor) //접촉한 것이 액터라면
	{
		AMain* Main = Cast<AMain>(OtherActor); // 접촉한 액터를 Main으로 변환시킨다.
		if(Main)
		{
			Main->IncrementCoins(CoinCount);

			Destroy(); // 무기가 사라지면 안되니까 따로 만들어주기
		}
	}
}


void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);

	UE_LOG(LogTemp,Warning,TEXT("Pickup::OnOverlapBegin()"));
}