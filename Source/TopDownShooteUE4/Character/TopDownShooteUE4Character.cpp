// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownShooteUE4Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TopDownShooteUE4/TPSGameInstance.h"



ATopDownShooteUE4Character::ATopDownShooteUE4Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;





	GetCharacterMovement()->MaxWalkSpeed = CurrentCharacterSpeed.Run_Speed;//base speed




	// Creating volumesphere and attaching
	VolumeSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("VolumeSphereComponent"));
	VolumeSphereComponent->SetupAttachment(RootComponent);

	// Setup params of volume sphere
	VolumeSphereComponent->SetWorldScale3D(FVector(CurrentVolumeSphereSize, CurrentVolumeSphereSize, CurrentVolumeSphereSize)); 
	VolumeSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	VolumeSphereComponent->SetVisibility(true); 
	
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	CharHealthComponent = CreateDefaultSubobject<UTPSCharacterHealthComponent>(TEXT("HealthComponent"));
	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATopDownShooteUE4Character::InitWeapon);
	}
	if (CharHealthComponent)
	{
		CharHealthComponent->OnDead.AddDynamic(this, &ATopDownShooteUE4Character::CharDead);
	}
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATopDownShooteUE4Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if (myPC)
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();

			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}

	//Check character can sprint
	if (FMath::Abs(AxisX) > KINDA_SMALL_NUMBER || FMath::Abs(AxisY) > KINDA_SMALL_NUMBER) //Check is character is moving
	{
		FVector MovementDirection = FVector(AxisX, AxisY, 0.0f);
		FRotator MovementRotation = MovementDirection.Rotation();

		FRotator CharacterRotation = GetActorRotation();

		float AngleDifference = FMath::Abs(MovementRotation.Yaw - CharacterRotation.Yaw);
		AngleDifference = FMath::UnwindDegrees(AngleDifference); 

		//bCanSprint = AngleDifference <= AngleDiaposonForSprint; too complex
		if (AngleDifference <= AngleDiaposonForSprint)
		{
			bCanSprint = true;
		}
		else
		{
			bCanSprint = false;
		}
	}



	MovementTick(DeltaSeconds);
	ReloadingStamina();
}


void ATopDownShooteUE4Character::BeginPlay()
{
	Super::BeginPlay();



	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
}

void ATopDownShooteUE4Character::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATopDownShooteUE4Character::InputAxisX); 
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATopDownShooteUE4Character::InputAxisY); 

	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATopDownShooteUE4Character::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATopDownShooteUE4Character::InputAttackReleased);
	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &ATopDownShooteUE4Character::TryReloadWeapon);

	NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATopDownShooteUE4Character::TrySwicthNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchPreviosWeapon"), EInputEvent::IE_Pressed, this, &ATopDownShooteUE4Character::TrySwitchPreviosWeapon);
}

void ATopDownShooteUE4Character::InputAxisX(float Value)
{
	//using in  SetupPlayerInputComponent 
	AxisX = Value;
}

void ATopDownShooteUE4Character::InputAxisY(float Value)
{
	// using in SetupPlayerInputComponent 
	AxisY = Value;
}

void ATopDownShooteUE4Character::InputAttackPressed()
{
	AttackCharEvent(true);
}

void ATopDownShooteUE4Character::InputAttackReleased()
{
	AttackCharEvent(false);
}

void ATopDownShooteUE4Character::MovementTick(float DeltaTime)
{
	
	//using in  Tick 119
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

	if (CurrentMovementState == EMovementState::SprintRun_State)
	{
		FVector myRotationVector = FVector(AxisX, AxisY, 0.0f);
		FRotator myRotator = myRotationVector.ToOrientationRotator();
		SetActorRotation((FQuat(myRotator)));
	}
	else
	{
		APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (myController)
		{
			FHitResult ResultHit;
			//myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);// bug was here Config\DefaultEngine.Ini
			myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

			float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
			SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));
			
			if (CurrentWeapon)
			{
				FVector Displacement = FVector(0);
				switch (CurrentMovementState)
				{
				case EMovementState::Aim_State:
					Displacement = FVector(0.0f, 0.0f, 160.0f);
					CurrentWeapon->ShouldReduceDispersion = true;
					break;
				case EMovementState::WalkAim_State:
					CurrentWeapon->ShouldReduceDispersion = true;
					Displacement = FVector(0.0f, 0.0f, 160.0f);
					break;
				case EMovementState::Walk_State:
					Displacement = FVector(0.0f, 0.0f, 120.0f);
					CurrentWeapon->ShouldReduceDispersion = false;
					break;
				case EMovementState::Run_State:
					Displacement = FVector(0.0f, 0.0f, 120.0f);
					CurrentWeapon->ShouldReduceDispersion = false;
					break;
				case EMovementState::SprintRun_State:
					break;
				default:
					break;
				}

				CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
				//aim cursor like 3d Widget?
			}
		}
	}
}




void ATopDownShooteUE4Character::CharacterUpdate()
{
	


	switch (CurrentMovementState)
	{
	case EMovementState::Aim_State:
		TargetSpeed = CurrentCharacterSpeed.Aim_Speed;
		break;
	case EMovementState::Walk_State:
		TargetSpeed = CurrentCharacterSpeed.Walk_Speed;
		break;
	case EMovementState::Run_State:
		TargetSpeed = CurrentCharacterSpeed.Run_Speed;
		break;
	case EMovementState::SprintRun_State:
		TargetSpeed = CurrentCharacterSpeed.SprintRun_Speed;
		break;
	case EMovementState::WalkAim_State:
		TargetSpeed = CurrentCharacterSpeed.WalkAim_Speed;
		break;
	default:
		break;
	}

	GetWorldTimerManager().SetTimer(SmoothSpeedChangeTimerHande, this, &ATopDownShooteUE4Character::SmoothChangeSpeed, TimeStepForSmoothSpeedChangeTimer, true);
	
}

void ATopDownShooteUE4Character::ChangeMovementState()
{

	if (!IsWalkEnabled && !IsSprintRunEnabled && !IsAimEnabled)
	{
		CurrentMovementState = EMovementState::Run_State;
	}
	else
	{
		if (IsSprintRunEnabled && CurrentStaminaAmount > 0 && bCanSprint)
		{
			
			IsAimEnabled = false;
			IsWalkEnabled = false;
			CurrentMovementState = EMovementState::SprintRun_State;
		}
		if (IsWalkEnabled && !IsSprintRunEnabled && IsAimEnabled)
		{
			CurrentMovementState = EMovementState::WalkAim_State;
		}
		else
		{
			if (IsWalkEnabled && !IsSprintRunEnabled && !IsAimEnabled)
			{
				CurrentMovementState = EMovementState::Walk_State;
			}
			else
			{
				if (!IsWalkEnabled && !IsSprintRunEnabled && IsAimEnabled)
				{
					CurrentMovementState = EMovementState::Aim_State;
				}
			}
		}
	}
	
	CharacterUpdate();
	ChangeVolumesSphereSize(5, 1);

	//Weapon state update
	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon(CurrentMovementState);
	}
}



void ATopDownShooteUE4Character::ReloadingStamina()
{
	//Tick()
	if (!IsSprintRunEnabled || !bCanSprint)
	{
		if (CurrentStaminaAmount < AllStaminaAmount)
		{
			
			if (IsWalkEnabled)
			{
				float AddingStamina = (AllStaminaAmount * 0.01) * CoefOfStaminaIncreasing * 1.5; 
				CurrentStaminaAmount = CurrentStaminaAmount + AddingStamina;
			}
			else
			{
				float AddingStamina = (AllStaminaAmount * 0.01) * CoefOfStaminaIncreasing;
				CurrentStaminaAmount = CurrentStaminaAmount + AddingStamina;
			}
			
			
		}
	}
	else
	{
		if (CurrentStaminaAmount > 0)
		{
			float DecreasingStamina = (AllStaminaAmount * 0.01) * CoefOfStaminaDecreasing; 
			CurrentStaminaAmount = CurrentStaminaAmount - DecreasingStamina;
		}
		else
		{
			IsSprintRunEnabled = false;
			bCanSprint = false;
			ChangeMovementState();
		}
	}
	CurrentStaminaAmount = FMath::Clamp(CurrentStaminaAmount, 0.0f, 100.0f);
	OnStaminaChanged.Broadcast(CurrentStaminaAmount);
}



void ATopDownShooteUE4Character::SmoothChangeSpeed()
{
	float CurrentSpeed = GetCharacterMovement()->MaxWalkSpeed;
	float DeltaSpeed = TargetSpeed - CurrentSpeed;
	float MaxChange = 5000 * GetWorld()->GetDeltaSeconds();


	if (DeltaSpeed != 0)
	{
		if (DeltaSpeed > 0)
			{
			
				float NewSpeed = FMath::Clamp(CurrentSpeed + MaxChange, 0.0f, TargetSpeed);
				GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
			}
			else if (DeltaSpeed < 0)
			{
				float NewSpeed = FMath::Clamp(CurrentSpeed - MaxChange, TargetSpeed, CurrentSpeed);
				GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
			}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SmoothSpeedChangeTimerHande);
	}
	
	
}

void ATopDownShooteUE4Character::ChangeVolumesSphereSize(float value,float MultiplyCoef)
{	
	CurrentVolumeSphereSize = (value * MultiplyCoef) / 20 ;	
	VolumeSphereComponent->SetWorldScale3D(FVector(CurrentVolumeSphereSize, CurrentVolumeSphereSize, CurrentVolumeSphereSize)); 
}


void ATopDownShooteUE4Character::AttackCharEvent(bool bIsFiring)
{
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		//ToDo Check melee or range
		myWeapon->SetWeaponStateFire(bIsFiring);
		WeaponFireStart_BP(nullptr);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
}

AWeaponDefault* ATopDownShooteUE4Character::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATopDownShooteUE4Character::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if (myGI)
	{
		if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{
			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					CurrentWeapon = myWeapon;

					myWeapon->WeaponSetting = myWeaponInfo;

					//myWeapon->AdditionalWeaponInfo.Round = myWeaponInfo.MaxRound;

					myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
					myWeapon->UpdateStateWeapon(CurrentMovementState);

					myWeapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
					//if(InventoryComponent)
					CurrentIndexWeapon = NewCurrentIndexWeapon;//fix

					//Not Forget remove delegate on change/drop weapon
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATopDownShooteUE4Character::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATopDownShooteUE4Character::WeaponReloadEnd);

					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATopDownShooteUE4Character::WeaponFireStart);

					// after switch try reload weapon if needed
					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
					{
						CurrentWeapon->InitReload();
					}
						

					if (InventoryComponent)
					{
						InventoryComponent->OnWeaponAmmoAviable.Broadcast(myWeapon->WeaponSetting.WeaponType);
					}
						
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::InitWeapon - Weapon not found in table -NULL"));
		}
	}

}

UDecalComponent* ATopDownShooteUE4Character::GetCursorToWorld()
{
	return CurrentCursor;
}

void ATopDownShooteUE4Character::TryReloadWeapon()
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponRound() <= CurrentWeapon->WeaponSetting.MaxRound)
			CurrentWeapon->InitReload();
	}
}

void ATopDownShooteUE4Character::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

//void ATopDownShooteUE4Character::WeaponReloadStart_BP(UAnimMontage* Anim)
//{
//}

void ATopDownShooteUE4Character::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}
	WeaponReloadEnd_BP(bIsSuccess);
}

void ATopDownShooteUE4Character::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	// in BP
}

void ATopDownShooteUE4Character::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	// in BP
}

void ATopDownShooteUE4Character::WeaponFireStart(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	WeaponFireStart_BP(Anim);
}

void ATopDownShooteUE4Character::WeaponFireStart_BP_Implementation(UAnimMontage* Anim)
{
	// in BP
}

void ATopDownShooteUE4Character::TrySwicthNextWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		//We have more then one weapon go switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			//if (CurrentWeapon->WeaponReloading) Maybe block switchin during reloading
				//CurrentWeapon->CancelReload();
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
			{
			}
		}
	}
	TrySwitchWeaponBP();
}

void ATopDownShooteUE4Character::TrySwitchPreviosWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		//We have more then one weapon go switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
		//	if (CurrentWeapon->WeaponReloading) Maybe block switching during reloading
		//		CurrentWeapon->CancelReload();
		}

		if (InventoryComponent)
		{
			//InventoryComponent->SetAdditionalInfoWeapon(OldIndex, GetCurrentWeapon()->AdditionalWeaponInfo);
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
			{
			}
		}
	}
	TrySwitchWeaponBP();
}



void ATopDownShooteUE4Character::CharDead()
{
	float TimeAnim = 0.0f;
	int32 rnd = FMath::RandHelper(DeadsAnim.Num());
	if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
	{
		TimeAnim = DeadsAnim[rnd]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
	}

	bIsAlive = false;

	UnPossessed();

	//Timer rag doll
	GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ATopDownShooteUE4Character::EnableRagdoll, TimeAnim, false);

	GetCursorToWorld()->SetVisibility(false);
}

void ATopDownShooteUE4Character::EnableRagdoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

float ATopDownShooteUE4Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsAlive)
	{
		CharHealthComponent->ChangeHealthValue(-DamageAmount);
	}
	UE_LOG(LogTemp, Error, TEXT("Take Damage Character 635"));
	return ActualDamage;
}


void ATopDownShooteUE4Character::TrySwitchWeaponBP_Implementation()
{
}