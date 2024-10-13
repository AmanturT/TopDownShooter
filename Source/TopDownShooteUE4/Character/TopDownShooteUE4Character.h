// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TopDownShooteUE4/FuncLibrary/Types.h"
#include "Components/SphereComponent.h"
#include "TopDownShooteUE4/WeaponDefault.h"
#include "TopDownShooteUE4/InventoryComponent.h"
#include "TopDownShooteUE4/TPSCharacterHealthComponent.h"
#include "TopDownShooteUE4Character.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, NewStamina);
UCLASS(Blueprintable)
class TOPDOWNSHOOTEUE4_API ATopDownShooteUE4Character : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ATopDownShooteUE4Character();
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStaminaChanged OnStaminaChanged;
	FTimerHandle TimerHandle_RagDollTimer;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	class UTPSCharacterHealthComponent* CharHealthComponent;
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UDecalComponent* CursorToWorld;


public:
	//Weapon
	AWeaponDefault* CurrentWeapon = nullptr;
	//for demo 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FName InitWeaponName;


	//Cursor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UDecalComponent* CurrentCursor = nullptr;
	//Inputs
	UFUNCTION()
	void InputAxisX(float Value);

	UFUNCTION()
	void InputAxisY(float Value);
	UFUNCTION()
	void InputAttackPressed();

	UFUNCTION()
	void InputAttackReleased();

	float AxisX = 0.0f;
	float AxisY = 0.0f;

	//Movement
	UFUNCTION()
	void MovementTick(float DeltaTime);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState CurrentMovementState = EMovementState::Run_State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed CurrentCharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsSprintRunEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsWalkEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsAimEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bCanSprint = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsAlive = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<UAnimMontage*> DeadsAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AllStaminaAmount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentStaminaAmount;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Movement")
	float CoefOfStaminaDecreasing = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CoefOfStaminaIncreasing = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TimeStepForSmoothSpeedChangeTimer = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AngleDiaposonForSprint = 45;

	FTimerHandle SmoothSpeedChangeTimerHande; 

	float TargetSpeed = 0.0f;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	float CurrentVolumeSphereSize = 50.0f; 

	//Weapon funcs
	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWeaponName,FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon);

	UFUNCTION(BlueprintCallable)
	void TryReloadWeapon();

	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* Anim);

	UFUNCTION()
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe);

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess);

	UFUNCTION()
	void WeaponFireStart(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponFireStart_BP(UAnimMontage* Anim);
	//Cursor funcs
	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();



	//Movement Funcs

	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState();
    
	UFUNCTION(BlueprintCallable)
	void ReloadingStamina();

	UFUNCTION(BlueprintCallable)
	void SmoothChangeSpeed(); 

	//inventory
	void TrySwicthNextWeapon();
	void TrySwitchPreviosWeapon();
	UFUNCTION(BlueprintNativeEvent)
	void TrySwitchWeaponBP();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CurrentIndexWeapon = 0;

	UFUNCTION()
	void CharDead();
	void EnableRagdoll();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};

