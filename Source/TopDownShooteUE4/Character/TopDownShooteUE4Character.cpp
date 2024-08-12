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

	// Create a decal in the world to show the cursor's location
	/*CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Blueprint/Characters/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());*/

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
	
}

void ATopDownShooteUE4Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	//if (CursorToWorld != nullptr)
	//{
	//	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	//	{
	//		if (UWorld* World = GetWorld())
	//		{
	//			FHitResult HitResult;
	//			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
	//			FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
	//			FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
	//			Params.AddIgnoredActor(this);
	//			World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
	//			FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
	//			CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
	//		}
	//	}
	//	else if (APlayerController* PC = Cast<APlayerController>(GetController()))
	//	{
	//		FHitResult TraceHitResult;
	//		PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
	//		FVector CursorFV = TraceHitResult.ImpactNormal;
	//		FRotator CursorR = CursorFV.Rotation();
	//		CursorToWorld->SetWorldLocation(TraceHitResult.Location);
	//		CursorToWorld->SetWorldRotation(CursorR);
	//	}
	//}
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

	MovementTick(DeltaSeconds);
	ReloadingStamina();
}


void ATopDownShooteUE4Character::BeginPlay()
{
	Super::BeginPlay();

	InitWeapon();

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
}

void ATopDownShooteUE4Character::InputAxisX(float Value)
{
	//using in  SetupPlayerInputComponent 100
	AxisX = Value;
}

void ATopDownShooteUE4Character::InputAxisY(float Value)
{
	// using in SetupPlayerInputComponent 101
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
	
	//using in  Tick 93
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f),AxisX);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

	APlayerController* MyController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (MyController)
	{
		if (CurrentMovementState != EMovementState::SprintRun_State)
		{
			FHitResult HitResult;
			//myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);// bug was here Config\DefaultEngine.Ini
			MyController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, HitResult);



			float GetYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location).Yaw;
			SetActorRotation(FQuat(FRotator(0.0f, GetYaw, 0.0f)));
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
	if (!IsSprintRunEnabled)
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
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
}

AWeaponDefault* ATopDownShooteUE4Character::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATopDownShooteUE4Character::InitWeapon()
{
	if (InitWeaponClass)
	{
		FVector SpawnLocation = FVector(0);
		FRotator SpawnRotation = FRotator(0);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();

		AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(InitWeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
		if (myWeapon)
		{
			FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
			myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
			CurrentWeapon = myWeapon;

			myWeapon->UpdateStateWeapon(CurrentMovementState);
		}
	}
}

UDecalComponent* ATopDownShooteUE4Character::GetCursorToWorld()
{
	return CurrentCursor;
}

