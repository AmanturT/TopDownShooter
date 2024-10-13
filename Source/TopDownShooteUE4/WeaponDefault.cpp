

#include "WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AWeaponDefault::AWeaponDefault()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);


	MagazineDropSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("MagazineDropSpawnPoint"));
	MagazineDropSpawnPoint->SetupAttachment(RootComponent);


	ShellDropSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SleeveDropSpawnPoint"));
	ShellDropSpawnPoint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();

	WeaponInit();

}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	

	//Before fire we check if we can fire(reloading anim or block fire) after we use fire().
	// If block fire is true we minus deltaTime from FireTime and when fire timer<= 0 we change block fire to false. 
	//because of delay between fire(when player tap fire single times every single time weapon fire without any delay
	if (BlockFire)
	{
		FireTimer -= DeltaTime;
		if (FireTimer <= 0.f)
		{
			BlockFire = false; 
		}
		return;
	}
	if (GetWeaponRound() > 0)
	{
		if (WeaponFiring)
			if (FireTimer <= 0.f)
			{
				if (!WeaponReloading)
				{
					Fire();
				}
			}
			else
			{
				FireTimer -= DeltaTime;
			}
				
	}

}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			else
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
		}

		if (CurrentDispersion < CurrentDispersionMin)
		{

			CurrentDispersion = CurrentDispersionMin;

		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
	//if (ShowDebug)
	//	UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}

	UpdateStateWeapon(EMovementState::Run_State);
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}	
	else
	{
		WeaponFiring = false;
	}
		
	
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFireAim;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFire;

	if (WeaponSetting.AnimWeaponInfo.AnimWeaponFire
		&& SkeletalMeshWeapon
		&& SkeletalMeshWeapon->GetAnimInstance())//Bad Code? maybe best way init local variable or in func
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSetting.AnimWeaponInfo.AnimWeaponFire);
	}




	FireTimer = WeaponSetting.RateOfFire;
	AdditionalWeaponInfo.Round = AdditionalWeaponInfo.Round - 1;
	ChangeDispersionByShot();

	OnWeaponFireStart.Broadcast(AnimToPlay);

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentTransform());

	int8 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		FVector EndLocation;
		for (int8 i = 0; i < NumberProjectile; i++)//Shotgun
		{
			EndLocation = GetFireEndLocation();

			if (ProjectileInfo.Projectile)
			{
				//Projectile Init ballistic fire
				FVector Dir = EndLocation - SpawnLocation;

				Dir.Normalize();

				FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
				SpawnRotation = myMatrix.Rotator();



				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile)
				{
					myProjectile->InitProjectile(WeaponSetting.ProjectileSetting,ProjectileInfo.BulletMesh,ProjectileInfo.BulletFX);
				}
			}
			else
			{
				FHitResult Hit;
				TArray<AActor*> Actors;

				EDrawDebugTrace::Type DebugTrace;
				if (ShowDebug)
				{
					DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShootLocation->GetForwardVector() * WeaponSetting.DistacneTrace, FColor::Black, false, 5.f, (uint8)'\000', 0.5f);
					DebugTrace = EDrawDebugTrace::ForDuration;
				}
				else
					DebugTrace = EDrawDebugTrace::None;

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation * WeaponSetting.DistacneTrace,
					ETraceTypeQuery::TraceTypeQuery4, false, Actors, DebugTrace, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

				if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
				{
					EPhysicalSurface mySurfacetype = UGameplayStatics::GetSurfaceType(Hit);

					if (WeaponSetting.ProjectileSetting.HitDecals.Contains(mySurfacetype))
					{
						UMaterialInterface* myMaterial = WeaponSetting.ProjectileSetting.HitDecals[mySurfacetype];

						if (myMaterial && Hit.GetComponent())
						{
							UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), Hit.GetComponent(), NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
						}
					}
					if (WeaponSetting.ProjectileSetting.HitFXs.Contains(mySurfacetype))
					{
						UParticleSystem* myParticle = WeaponSetting.ProjectileSetting.HitFXs[mySurfacetype];
						if (myParticle)
						{
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
						}
					}

					if (WeaponSetting.ProjectileSetting.HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSetting.ProjectileSetting.HitSound, Hit.ImpactPoint);
					}

					UGameplayStatics::ApplyDamage(Hit.GetActor(), WeaponSetting.ProjectileSetting.ProjectileDamage, GetInstigatorController(), this, NULL);
				}

			}
		}
	}
	FireTimer = WeaponSetting.RateOfFire;
	BlockFire = true; 

	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		//Init Reload
		if (CheckCanWeaponReload())
			InitReload();
	}
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	//ToDo Dispersion
	BlockFire = false;

	switch (NewMovementState)
	{
		case EMovementState::Aim_State:
		{
			WeaponAiming = true;
			CurrentDispersionMax = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMax;
			CurrentDispersionMin = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMin;
			CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimRecoil;
			CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
			break;
		}
		case EMovementState::WalkAim_State:
		{
			WeaponAiming = true;
			CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMax;
			CurrentDispersionMin = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMin;
			CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimRecoil;
			CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
			break;
		}
		case EMovementState::Walk_State:
		{
			WeaponAiming = false;
			CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
			CurrentDispersionMin = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
			CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
			CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
			break;
		}
		case EMovementState::Run_State:
		{
			WeaponAiming = false;
			CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMax;
			CurrentDispersionMin = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMin;
			CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Run_StateDispersionAimRecoil;
			CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
			break;
		}
		case EMovementState::SprintRun_State:
		{
			WeaponAiming = false;
			BlockFire = true;
			SetWeaponStateFire(false);//set fire trigger to false
			//Block Fire
			break;
		}
		default:
		{
			break;
		}
	}
}

void AWeaponDefault::ChangeDispersionByShot()
{
	CurrentDispersion = CurrentDispersion + CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	//UE_LOG(LogTemp, Warning, TEXT("Vector: X = %f. Y = %f. Size = %f"), tmpV.X, tmpV.Y, tmpV.Size());

	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSetting.DistacneTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSetting.DistacneTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}


	if (ShowDebug)
	{
		//direction weapon look
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 5.f, (uint8)'\000', 0.5f);
		//direction projectile must fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 5.f, (uint8)'\000', 0.5f);
		//Direction Projectile Current fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black, false, 5.f, (uint8)'\000', 0.5f);

		//DrawDebugSphere(GetWorld(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector()*SizeVectorToChangeShootDirectionLogic, 10.f, 8, FColor::Red, false, 4.0f);
	}


	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShot() const
{
	return WeaponSetting.NumberProjectileByShot;
}


int32 AWeaponDefault::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}



void AWeaponDefault::InitReload()
{
	if(!BlockReloading && !WeaponReloading)
	{
		WeaponReloading = true;

		ReloadTimer = WeaponSetting.ReloadTime;

			//ToDo Anim reload
		if (WeaponSetting.AnimWeaponInfo.AnimCharReload)
		{
			OnWeaponReloadStart.Broadcast(WeaponSetting.AnimWeaponInfo.AnimCharReload);
		}
			
	}

}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;

	int8 AviableAmmoFromInventory = GetAviableAmmoForReload();
	int8 AmmoNeedTakeFromInv;
	int8 NeedToReload = WeaponSetting.MaxRound - AdditionalWeaponInfo.Round;

	if (NeedToReload > AviableAmmoFromInventory)
	{
		AdditionalWeaponInfo.Round = AviableAmmoFromInventory;
		AmmoNeedTakeFromInv = AviableAmmoFromInventory;
	}
	else
	{
		AdditionalWeaponInfo.Round += NeedToReload;
		AmmoNeedTakeFromInv = NeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInv);
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool result = true;
	if (GetOwner())
	{
		UInventoryComponent* MyInv = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (MyInv)
		{
			int8 AviableAmmoForWeapon;
			if (!MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				result = false;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("CheckCanWeaponReload result: %s"), result ? TEXT("true") : TEXT("false"));
	return result;
}

int8 AWeaponDefault::GetAviableAmmoForReload()
{
	int8 AviableAmmoForWeapon = WeaponSetting.MaxRound;
	if (GetOwner())
	{
		UInventoryComponent* MyInv = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (MyInv)
		{			
			if (MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				AviableAmmoForWeapon = AviableAmmoForWeapon;
			}
		}
	}
	return AviableAmmoForWeapon;
}

void AWeaponDefault::SpawnMagazineDrop()
{
	if (WeaponSetting.MagazineDrop) 
	{
	
		FVector SpawnLocation = MagazineDropSpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = MagazineDropSpawnPoint->GetComponentRotation();
		FActorSpawnParameters SpawnParams;

	
		UStaticMeshComponent* SpawnedMagazine = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		if (SpawnedMagazine)
		{
			SpawnedMagazine->SetStaticMesh(WeaponSetting.MagazineDrop);
			SpawnedMagazine->SetWorldLocation(SpawnLocation);
			SpawnedMagazine->SetWorldRotation(SpawnRotation);
			
			SpawnedMagazine->RegisterComponent();
			SpawnedMagazine->SetSimulatePhysics(true);
			//SpawnedMagazine->AddImpulse(SpawnRotation.Vector() * 500.0f);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [SpawnedMagazine]()
				{
					if (SpawnedMagazine)
					{
						SpawnedMagazine->DestroyComponent();
					}
				}, 5, false);
		}
	
	}
	
}

void AWeaponDefault::SpawnShellDrop()
{
	if (WeaponSetting.ShellBullets)
	{

		FVector SpawnLocation = ShellDropSpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = ShellDropSpawnPoint->GetComponentRotation();
		FActorSpawnParameters SpawnParams;


		UStaticMeshComponent* SpawnedShell = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		if (SpawnedShell)
		{
			SpawnedShell->SetStaticMesh(WeaponSetting.ShellBullets);
			SpawnedShell->SetWorldLocation(SpawnLocation);
			SpawnedShell->SetWorldRotation(SpawnRotation);
			
			SpawnedShell->RegisterComponent();
			SpawnedShell->SetSimulatePhysics(true);
			SpawnedShell->AddImpulse(SpawnRotation.Vector() * WeaponSetting.ImpusleOfShellDrop);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [SpawnedShell]()
				{
					if (SpawnedShell)
					{
						SpawnedShell->DestroyComponent();
					}
				}, 5, false);
		}
	}
	
}

