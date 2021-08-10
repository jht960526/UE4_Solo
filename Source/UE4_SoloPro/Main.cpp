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
	GetCapsuleComponent()->SetCapsuleSize(48.f,105.f); // 굳이? 하드코딩을? 블프에서 하면될듯

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
	// 자동으로 캐릭터의 이동방향을 움직이는 방향에 맞춰줌. 회전보간
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

	StaminaDrainRate = 25.f; // 초당 줄어드는 스테미나 비율
	MinSprintStamina = 50.f; // 색이 바뀌는 구간

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
	 UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //애니메이션 가져오기
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

	// Tick이 돌아갈때 스테미나도 줄어들어야하니까
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:

		if(bShiftKeyDown)
		{
			if(Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina; // 줄어든 값을 적용하기 위함
			}
			else
			{
				Stamina -= DeltaStamina; // 줄어든 값을 적용하기 위함
			}
			SetMovementStatus(EMovementStatus::EMS_Sprinting); // 키가 눌렸으니까 달리기 상태
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

	case EStaminaStatus::ESS_BelowMinimum: // 노랑색 되었을때
		if(bShiftKeyDown)
		{
			if(Stamina - DeltaStamina <= 0.f)  // 스테미나가 0보다 작거나 같을 때
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else // 그게 아닐때는 계속 달리기
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
		SetMovementStatus(EMovementStatus::EMS_Normal); // 지친 상태니까 일반상태로
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
		SetMovementStatus(EMovementStatus::EMS_Normal); // 회복단계라 일반상태
		break;

	default:
		break;

	}

	if(bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation()); // 적위치 가져오기
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// 캐릭터에서 가져온 함수 사용
	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	// 우리가 만든 함수 사용
	PlayerInputComponent->BindAxis("MoveForward",this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this, &AMain::MoveRight);

	// 폰에는 마우스로 컨트롤러 조절하는 함수가 있기 때문에 받아옴
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
		// FRotationMatrix : 캐릭터의 중심이 0,0,0인 상태에서 각 축에 따라 얼마나 회전되어 있는지를 나타냄
		// 회전에 따라 변경되는 축들
		// Get UnitAxis: 회전방향에 대한 절대 축 얻기
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
			Weapon->Equip(this); // 무기 장착하기
			SetActiveOverlappingItem(nullptr); // 무기 장착하면 오버렙 가능한 무기null
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
	if(EquippedWeapon)  // 새로운 무기가 장착되면 이전 무기 파괴시킴
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

	    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //애니메이션 가져오기
	    if(AnimInstance && CombatMontage)
	    {
			int32 Section = FMath::RandRange(0,1); // rand
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage,2.2f); // 애니메이션 몽타주 플레이(어떤거,속도)
		        AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage); //어떤 애님으로 넘어갈건지
				break;

			case 1:
				AnimInstance->Montage_Play(CombatMontage,1.8f); // 애니메이션 몽타주 플레이(어떤거,속도)
		        AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage); //어떤 애님으로 넘어갈건지
				break;

			default:
				;
			}
		   

	    }
		//if(EquippedWeapon->SwingSound)
		//{
		//	UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound); //바로 실행해 버려서 조금 아쉬움
		//}
	}
	
}

void AMain::AttackEnd() // 에디터에서 사용가능
{
	bAttacking = false; // 순서를 바꿔야 하지 않을까?라는 의문
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

