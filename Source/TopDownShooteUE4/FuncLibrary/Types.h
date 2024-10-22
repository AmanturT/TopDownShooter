// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "Types.generated.h"

class AProjectileDefault;
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Run_State UMETA(DisplayName = "Run State"),
	WalkAim_State UMETA(DisplayName = "WalkAim State"),
	SprintRun_State UMETA(DisplayName = "SprintRun State")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	RifleType UMETA(DisplayName = "Rifle"),
	ShotGunType UMETA(DisplayName = "ShotGun"),
	SniperRifle UMETA(DisplayName = "SniperRifle"),
	GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),
	RocketLauncher UMETA(DisplayName = "RocketLauncher")
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	Helmet UMETA(DisplayName = "Helmet"),
	Chest UMETA(DisplayName = "Chest"),
	Pants UMETA(DisplayName = "Pants"),
	Boots UMETA(DisplayName = "Boots")
};
USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Movement")
	float Aim_Speed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Walk_Speed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Run_Speed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintRun_Speed = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkAim_Speed = 100.0f;
};


USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	TSubclassOf<class AProjectileDefault> Projectile = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	UStaticMesh* BulletMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	UParticleSystem* BulletFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileLifeTime = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileInitSpeed = 2000.0f;

	//material to decal on hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;
	//Sound when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	USoundBase* HitSound = nullptr;
	//fx when hit check by surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	UParticleSystem* ExploseFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	USoundBase* ExploseSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileMaxRadiusDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileMinRadiusDamage = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ExploseMaxDamage = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float MaxDistanceForMaxDamage = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float CoefOfDamageDecreaseByDistance = 0.5f;

	//Timer add

};


USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionReduction = .3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float AimWalk_StateDispersionAimMax = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float AimWalk_StateDispersionAimMin = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float AimWalk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float AimWalk_StateDispersionReduction = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Run_StateDispersionAimMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Run_StateDispersionAimMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Run_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Run_StateDispersionReduction = 0.1f;
};


USTRUCT(BlueprintType)
struct FAnimationWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
	UAnimMontage* AnimCharFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
	UAnimMontage* AnimCharFireAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
	UAnimMontage* AnimCharReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
	UAnimMontage* AnimCharReloadAim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
	UAnimMontage* AnimWeaponReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
	UAnimMontage* AnimWeaponReloadAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
	UAnimMontage* AnimWeaponFire = nullptr;
};

USTRUCT(BlueprintType)
struct FDropMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	UStaticMesh* DropMesh = nullptr;
	//0.0f immediately drop
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
	float DropMeshTime = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
	float DropMeshLifeTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
	FTransform DropMeshOffset = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
	FVector DropMeshImpulseDir = FVector(0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
	float PowerImpulse = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
	float ImpulseRandomDispersion = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
	float CustomMass = 0.0f;
};


USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<class AWeaponDefault> WeaponClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float ReloadTime = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 MaxRound = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 NumberProjectileByShot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	FWeaponDispersion DispersionWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
	USoundBase* SoundFireWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
	USoundBase* SoundReloadWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX ")
	UParticleSystem* EffectFireWeapon = nullptr;
	//if null use trace logic (TSubclassOf<class AProjectileDefault> Projectile = nullptr)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile ")
	FProjectileInfo ProjectileSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
	float WeaponDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
	float DistacneTrace = 2000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect ")
	UDecalComponent* DecalOnHit = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim ")
	FAnimationWeaponInfo AnimWeaponInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
	class UStaticMesh* MagazineDrop = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
	UStaticMesh* ShellBullets = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
	float ImpusleOfShellDrop = 0;

	//inv
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
	float SwitchTimeToWeapon = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
	UTexture2D* WeaponIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
	UTexture2D* ProjectileIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
	EWeaponType WeaponType = EWeaponType::RifleType;
};

USTRUCT(BlueprintType)
struct FArmorPieceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float ArmorPoints; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	EArmorType Type = EArmorType::Chest;
	//Nvg-NightVisionGoggles for Helmets only
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helmet")
	bool DoesHaveNvg = false; 
	//SpeedBonus for pants or boots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boots")
	float SpeedBonus = 0.0f;  
};
USTRUCT(BlueprintType)
struct FAdditionalWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int32 Round = 10;
};

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
	FName NameItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
	struct FAdditionalWeaponInfo AdditionalInfo;
};
USTRUCT(BlueprintType)
struct FAmmoSlot
{
	GENERATED_BODY()

	///Index Slot by Index Array
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
	EWeaponType WeaponType = EWeaponType::RifleType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
	int32 Cout = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
	int32 MaxCout = 100;
};

USTRUCT(BlueprintType)
struct FDropItem : public FTableRowBase
{
	GENERATED_BODY()

	///Index Slot by Index Array
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropWeapon")
	UStaticMesh* WeaponStaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropWeapon")
	USkeletalMesh* WeaponSkeletMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropWeapon")
	FWeaponSlot WeaponInfo;
};
UCLASS()
class UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

};