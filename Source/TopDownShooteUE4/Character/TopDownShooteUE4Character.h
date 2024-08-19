// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TopDownShooteUE4/FuncLibrary/Types.h"
#include "Components/SphereComponent.h"
#include "TopDownShooteUE4/WeaponDefault.h"
#include "TopDownShooteUE4Character.generated.h"


UCLASS(Blueprintable)
class ATopDownShooteUE4Character : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ATopDownShooteUE4Character();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* VolumeSphereComponent; //Sound Sphere smt like sound sim

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
	//Stealth
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	float CurrentVolumeSphereSize = 50.0f; 

	//Weapon funcs
	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWeaponName);

	UFUNCTION(BlueprintCallable)
	void TryReloadWeapon();

	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* Anim);

	UFUNCTION()
	void WeaponReloadEnd();

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP();


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

	//Stealth funcs
	UFUNCTION(BlueprintCallable)
	void ChangeVolumesSphereSize(float value,float MultiplyCoef);


};

