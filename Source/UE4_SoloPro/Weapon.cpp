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

	if((WeaponState == EWeaponState::EMS_Pickup) && OtherActor) // �Ⱦ������� ���Ϳ� ������ �Ǿ�����
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

	if(OtherActor) // ���Ϳ� ������ �Ǿ�����
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
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // ���⿡�� ī�޶� �� ����
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // ��Ŭ������ ����

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket"); // �����Ϳ��� ������ �����̸��� ã�Ƽ� ������

		if(RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Char->GetMesh()); //ĳ���Ϳ� �̰��� ����
			bRotate = false;

			//Char->GetEquippedWeapon()->Destroy(this); // ���ο� ���⸦ �����ϱ� ���� ���� ���� �ı�
			Char->SetEquippedWeapon(this); // ���� ���̱�
			Char->SetActiveOverlappingItem(nullptr); // ���� ���������ϱ� �������� ���� ���ֱ�
		}
		if(OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound); //���� ���忡 ���� ������ �Ҹ�on
		if(!bWeaponParticles)
		{
			IdleParticlesComponent->Deactivate();
		}
	}
}
