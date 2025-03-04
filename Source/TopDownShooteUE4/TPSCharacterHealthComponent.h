// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSHealthComponent.h"
#include "TPSCharacterHealthComponent.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange, float, Shield, float, Damage);

UCLASS()
class TOPDOWNSHOOTEUE4_API UTPSCharacterHealthComponent : public UTPSHealthComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
	FOnShieldChange OnShieldChange;

	FTimerHandle TimerHandle_CollDownShieldTimer;
	FTimerHandle TimerHandle_ShieldRecoveryRateTimer;
protected:

	float Shield = 100.0f;
	virtual void BeginPlay() override;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float CoolDownShieldRecoverTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoverValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoverRate = 0.1f;
	void UpdateCoefOfDamage(float value);
	void ChangeHealthValue(float ChangeValue) override;
	UFUNCTION(BlueprintCallable)
	float GetCurrentShield();

	void ChangeShieldValue(float ChangeValue);

	void CoolDownShieldEnd();

	void RecoveryShield();
};
