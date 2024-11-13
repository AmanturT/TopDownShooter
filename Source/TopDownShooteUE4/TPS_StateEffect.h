// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Animation/AnimMontage.h"
#include "Components/PostProcessComponent.h"
#include "TPS_StateEffect.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class TOPDOWNSHOOTEUE4_API UTPS_StateEffect : public UObject
{
	GENERATED_BODY()
public:

	virtual bool InitObject(AActor* Actor);
	virtual void DestroyObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	bool bIsStakable = false;

	AActor* myActor = nullptr;
};

UCLASS()
class TOPDOWNSHOOTEUE4_API UTPS_StateEffect_ExecuteOnce : public UTPS_StateEffect
{
	GENERATED_BODY()

public:
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Once")
	float Power = 20.0f;
};

UCLASS()
class TOPDOWNSHOOTEUE4_API UTPS_StateEffect_ExecuteTimer : public UTPS_StateEffect
{
	GENERATED_BODY()

public:
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void Execute();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	float Power = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	float Timer = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	float RateTime = 1.0f;

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	UNiagaraSystem* ParticleEffect = nullptr;

};



UCLASS()
class TOPDOWNSHOOTEUE4_API UTPS_StateEffect_Stun : public UTPS_StateEffect_ExecuteTimer
{
	GENERATED_BODY()

public:
	virtual bool InitObject(AActor* Actor) override;
	virtual void DestroyObject() override;
	virtual void Execute() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun Settings")
	UAnimMontage* StunAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun Settings")
	float AnimationPlayRate = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun Settings")
	UMaterialInterface* BlurPostProcessMaterial;


	void PlayStunAnimation();
	void StopStunAnimation();
};