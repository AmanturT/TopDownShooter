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


float UArmorBaseComponent::CalculateTotalArmorValue()
{
	return EquippedArmor.GetTotalArmorValue();
}


bool UArmorBaseComponent::EquipHelmet(FHelmet& NewHelmet, USkeletalMeshComponent* CharacterMesh)
{
	EquippedArmor.Helmet = NewHelmet;
	if (!CharacterMesh || !EquippedArmor.Helmet.HelmetMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid helmet info or character mesh."));
		return false;
	}

	
	if (UStaticMeshComponent** HelmetComponent = EquippedArmorComponents.Find(EquippedArmor.Helmet.HelmetBoneName))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is already helmet"));
		return false;
	}


	UStaticMeshComponent* NewHelmetComponent = NewObject<UStaticMeshComponent>(CharacterMesh);
	NewHelmetComponent->SetStaticMesh(EquippedArmor.Helmet.HelmetMesh);
	NewHelmetComponent->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquippedArmor.Helmet.HelmetBoneName);


	EquippedArmorComponents.Add(EquippedArmor.Helmet.HelmetBoneName, NewHelmetComponent);
	OnArmorValueChanged.Broadcast(CalculateTotalArmorValue());
	return true;
}

void UArmorBaseComponent::EquipChestplate(FBoots& NewBoots, USkeletalMeshComponent* CharacterMesh)
{
	EquippedArmor.Boots = NewBoots;
	OnArmorValueChanged.Broadcast(CalculateTotalArmorValue());
}

void UArmorBaseComponent::EquipBoots(FChestplate& NewChestplate, USkeletalMeshComponent* CharacterMesh)
{
	EquippedArmor.Chestplate = NewChestplate;
	OnArmorValueChanged.Broadcast(CalculateTotalArmorValue());
}


void UArmorBaseComponent::UnequipHelmet()
{
	EquippedArmor.Helmet = FHelmet();  
	if (UStaticMeshComponent** HelmetComponent = EquippedArmorComponents.Find(EquippedArmor.Helmet.HelmetBoneName))
	{
		if (*HelmetComponent)
		{
			(*HelmetComponent)->DestroyComponent();
		}
		EquippedArmorComponents.Remove(EquippedArmor.Helmet.HelmetBoneName);
	}


	OnArmorValueChanged.Broadcast(CalculateTotalArmorValue());
}
void UArmorBaseComponent::UnequipBoots()
{
	EquippedArmor.Boots = FBoots();
	OnArmorValueChanged.Broadcast(CalculateTotalArmorValue());
}
void UArmorBaseComponent::UnequipBodyArmor()
{
	EquippedArmor.Chestplate = FChestplate();
	OnArmorValueChanged.Broadcast(CalculateTotalArmorValue());
}
// Called every frame
void UArmorBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

