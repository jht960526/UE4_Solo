// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	bWeaponParticles = false;

	WeaponState = EWeaponState::EMS_Pickup;
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);

	if((WeaponState == EWeaponState::EMS_Pickup) && OtherActor) // 픽업상태의 액터와 오버랩 되었을때
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);

	if(OtherActor) // 액터와 오버랩 되었을때
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(AMain* Char)
{
	if(Char)
	{
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // 무기에는 카메라 줌 무시
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // 폰클래스도 무시

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket"); // 에디터에서 지정한 소켓이름을 찾아서 가져옴

		if(RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Char->GetMesh()); //캐릭터에 이것을 붙임
			bRotate = false;

			//Char->GetEquippedWeapon()->Destroy(this); // 새로운 무기를 장착하기 전에 이전 무기 파괴
			Char->SetEquippedWeapon(this); // 무기 붙이기
			Char->SetActiveOverlappingItem(nullptr); // 무기 장착했으니까 오버랩된 무기 없애기
		}
		if(OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound); //장착 사운드에 값이 있으면 소리on
		if(!bWeaponParticles)
		{
			IdleParticlesComponent->Deactivate();
		}
	}
}
