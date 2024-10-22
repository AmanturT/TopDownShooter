// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmorBaseComponent.h"

// Sets default values for this component's properties
UArmorBaseComponent::UArmorBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



// Called when the game starts
void UArmorBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UArmorBaseComponent::EquipArmorPiece(const FArmorPieceInfo& NewArmor)
{
	EquippedArmor.Add(NewArmor);

	//Broadcast for healthComponent(We must change coefOfDamage
	float TotalArmorValue = CalculateTotalArmorValue();
	OnArmorValueChanged.Broadcast(TotalArmorValue);
}
float UArmorBaseComponent::CalculateTotalArmorValue()
{
	float TotalValue = 0.0f;

	for (const FArmorPieceInfo& Armor : EquippedArmor)
	{
		TotalValue += Armor.ArmorPoints;
	}

	return TotalValue;
}
// Called every frame
void UArmorBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

