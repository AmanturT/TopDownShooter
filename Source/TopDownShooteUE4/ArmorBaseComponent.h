// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FuncLibrary/Types.h"
#include "ArmorBaseComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorValueChanged, float, NewArmorValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOPDOWNSHOOTEUE4_API UArmorBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// ArmorComponent manipulates equipped armor pieces 
	// and calculating the total armor value for the player. 
	// It uses delegate to send a signal when armor change to change coefOfDamage in health component
	UArmorBaseComponent();
	FOnArmorValueChanged OnArmorValueChanged;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
    TArray<FArmorPieceInfo> EquippedArmor;

    void EquipArmorPiece(const FArmorPieceInfo& NewArmor);

    float CalculateTotalArmorValue();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
