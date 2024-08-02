// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TopDownShooteUE4/FuncLibrary/Types.h"
#include "Components/SphereComponent.h"
#include "TopDownShooteUE4Character.generated.h"


UCLASS(Blueprintable)
class ATopDownShooteUE4Character : public ACharacter
{
	GENERATED_BODY()

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
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* VolumeSphereComponent; //����� �����
//��������������� ��� ��� ������ �� �������
public:
	//��� ����������
	//������� ��� X 
	UFUNCTION()
	void InputAxisX(float Value);
	//������� ��� Y
	UFUNCTION()
	void InputAxisY(float Value);

	float AxisX = 0.0f;
	float AxisY = 0.0f;

	//Tick Func ��� �� ��������
	UFUNCTION()
	void MovementTick(float DeltaTime);



	//��� ������������ ����������� ������������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState CurrentMovementState = EMovementState::Run_State;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed CurrentCharacterSpeed;


	//�����
	//���������� ��� ���������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsSprintRunEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsWalkEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsAimEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bCanSprint;


	// ���������� ��� �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AllStaminaAmount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentStaminaAmount;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Movement")
	float CoefOfStaminaDecreasing = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CoefOfStaminaIncreasing = 1;


	//���������� ��� ����� �������
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	float CurrentVolumeSphereSize = 50.0f; //����� ������� �������� ������ �� ������
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	float CurrentPlayerHeight; //������ ������(� ������ ���� �� �������� �� ������ ����������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth")
	bool IsPlayerHidded = false; //�������� �� ����� �� ��������



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TimeStepForSmoothSpeedChangeTimer = 0.1;

	FTimerHandle SmoothSpeedChangeTimerHande; 

	float TargetSpeed = 0.0f;
	// ������� ��� ���������

	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState();
    

	//������� ��� ������� � �������� ��������� ��������


	UFUNCTION(BlueprintCallable)
	void ReloadingStamina();

	UFUNCTION(BlueprintCallable)
	void SmoothChangeSpeed(); // ������� ��������� ��������

	//������� ��� ������
	UFUNCTION(BlueprintCallable)
	void ChangeVolumesSphereSize(float value,float MultiplyCoef);


};

