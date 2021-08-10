// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f,105.f); // ����? �ϵ��ڵ���? �������� �ϸ�ɵ�

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Don't rotate when the controller rotates.
	// Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	// �ڵ����� ĳ������ �̵������� �����̴� ���⿡ ������. ȸ������
	GetCharacterMovement()->RotationRate = FRotator(0.0f,540.f,0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bLMBDown = false;

	//Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f; // �ʴ� �پ��� ���׹̳� ����
	MinSprintStamina = 50.f; // ���� �ٲ�� ����

	InterpSpeed = 15.f;
	bInterpToEnemy = false;
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if(MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::DecrementHealth(float Amount)
{
	if(Health - Amount <= 0.f)
	{
		Health -= Amount; // healthbar update
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	DecrementHealth(DamageAmount);

	return DamageAmount;
}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMain::Die()
{
	 UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //�ִϸ��̼� ��������
	 if(AnimInstance && CombatMontage)
	 {
		 AnimInstance->Montage_Play(CombatMontage, 1.0f);
		 AnimInstance->Montage_JumpToSection(FName("Death"));
	 }
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	// Tick�� ���ư��� ���׹̳��� �پ�����ϴϱ�
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:

		if(bShiftKeyDown)
		{
			if(Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina; // �پ�� ���� �����ϱ� ����
			}
			else
			{
				Stamina -= DeltaStamina; // �پ�� ���� �����ϱ� ����
			}
			SetMovementStatus(EMovementStatus::EMS_Sprinting); // Ű�� �������ϱ� �޸��� ����
		}
		else // Shift key up
		{
			if(Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}

		break;

	case EStaminaStatus::ESS_BelowMinimum: // ����� �Ǿ�����
		if(bShiftKeyDown)
		{
			if(Stamina - DeltaStamina <= 0.f)  // ���׹̳��� 0���� �۰ų� ���� ��
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else // �װ� �ƴҶ��� ��� �޸���
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}

		else // Shift key up
		{
			if(Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_Exhausted:
		if(bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else // Shift key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal); // ��ģ ���´ϱ� �Ϲݻ��·�
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if(Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal); // ȸ���ܰ�� �Ϲݻ���
		break;

	default:
		break;

	}

	if(bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation()); // ����ġ ��������
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// ĳ���Ϳ��� ������ �Լ� ���
	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	// �츮�� ���� �Լ� ���
	PlayerInputComponent->BindAxis("MoveForward",this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this, &AMain::MoveRight);

	// ������ ���콺�� ��Ʈ�ѷ� �����ϴ� �Լ��� �ֱ� ������ �޾ƿ�
	PlayerInputComponent->BindAxis("Turn",this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate",this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate",this, &AMain::LookUpAtRate);

}

void AMain::MoveForward(float Value)
{
	if((Controller != nullptr) && (Value != 0.0f) && (!bAttacking))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// FRotationMatrix : ĳ������ �߽��� 0,0,0�� ���¿��� �� �࿡ ���� �󸶳� ȸ���Ǿ� �ִ����� ��Ÿ��
		// ȸ���� ���� ����Ǵ� ���
		// Get UnitAxis: ȸ�����⿡ ���� ���� �� ���
		AddMovementInput(Direction,Value);
	}
}

void AMain::MoveRight(float Value)
{
	if((Controller != nullptr) && (Value != 0.0f) && (!bAttacking))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction,Value);
	}
}

void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LMBDown()
{
	bLMBDown = true;

	if(ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if(Weapon)
		{
			Weapon->Equip(this); // ���� �����ϱ�
			SetActiveOverlappingItem(nullptr); // ���� �����ϸ� ������ ������ ����null
		}
	}
	else if(EquippedWeapon)
	{
		Attack();
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if(EquippedWeapon)  // ���ο� ���Ⱑ �����Ǹ� ���� ���� �ı���Ŵ
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
	if(!bAttacking)
	{
		bAttacking = true;
		SetInterpToEnemy(true);

	    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //�ִϸ��̼� ��������
	    if(AnimInstance && CombatMontage)
	    {
			int32 Section = FMath::RandRange(0,1); // rand
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage,2.2f); // �ִϸ��̼� ��Ÿ�� �÷���(���,�ӵ�)
		        AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage); //� �ִ����� �Ѿ����
				break;

			case 1:
				AnimInstance->Montage_Play(CombatMontage,1.8f); // �ִϸ��̼� ��Ÿ�� �÷���(���,�ӵ�)
		        AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage); //� �ִ����� �Ѿ����
				break;

			default:
				;
			}
		   

	    }
		//if(EquippedWeapon->SwingSound)
		//{
		//	UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound); //�ٷ� ������ ������ ���� �ƽ���
		//}
	}
	
}

void AMain::AttackEnd() // �����Ϳ��� ��밡��
{
	bAttacking = false; // ������ �ٲ�� ���� ������?��� �ǹ�
	SetInterpToEnemy(false);
	if(bLMBDown)
	{
		Attack();
	}
}

void AMain::PlaySwingSound()
{
	if(EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

