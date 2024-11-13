// Fill out your copyright notice in the Description page of Project Settings.


#include "TPS_StateEffect.h"
#include "TopDownShooteUE4/TPSHealthComponent.h"
#include "Animation/AnimInstance.h"
#include "TopDownShooteUE4/Character/TopDownShooteUE4Character.h"
#include "TPS_IGameActor.h"


bool UTPS_StateEffect::InitObject(AActor* Actor)
{

	myActor = Actor;

	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}

	return true;
}

void UTPS_StateEffect::DestroyObject()
{
	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}

	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
}





//-----------------------------------------------------------------------------------------{EXECUTE ONCE}----------------------------------------------------------------------------------------







bool UTPS_StateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);
	ExecuteOnce();
	return true;
}

void UTPS_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}

	DestroyObject();
}





//--------------------------------------------------------------------------------------{EXECUTE BY TIMER}------------------------------------------------------------------------------------------------------------------------------





bool UTPS_StateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTPS_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTPS_StateEffect_ExecuteTimer::Execute, RateTime, true);

	if (ParticleEffect)
	{
		FName NameBoneToAttached;
		FVector Loc = FVector(0);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			myActor->GetWorld(),
			ParticleEffect,
			myActor->GetActorLocation() + Loc,  
			FRotator::ZeroRotator,            
			FVector(1.0f)                    
		);
	}

	return true;
}

void UTPS_StateEffect_ExecuteTimer::DestroyObject()
{
	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{
		//UGameplayStatics::ApplyDamage(myActor,Power,nullptr,nullptr,nullptr);	
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}
}



//--------------------------------------------------------------{STUN EFFECT}------------------------------------------------------------------------------------------------------



bool UTPS_StateEffect_Stun::InitObject(AActor* Actor)
{
	
	Super::InitObject(Actor);
	UE_LOG(LogTemp, Error, TEXT("Init TPSEFFECT_STUN"));
	if (myActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Cast good TPSEFFECT_STUN"));
		ATopDownShooteUE4Character* myCharacter = Cast<ATopDownShooteUE4Character>(myActor);
		if(myCharacter)
		{
			myCharacter->bCanMove = false;
		}
	}
	PlayStunAnimation();

	return true;
}

void UTPS_StateEffect_Stun::DestroyObject()
{
	UE_LOG(LogTemp,Error,TEXT("DESTROY OBJECT TPSEFFECT_STUN"))
	StopStunAnimation();
	if (myActor)
	{
		ATopDownShooteUE4Character* myCharacter = Cast<ATopDownShooteUE4Character>(myActor);
		if (myCharacter)
		{
			myCharacter->bCanMove = true;
		}
	}
	
	Super::DestroyObject();
}

void UTPS_StateEffect_Stun::Execute()
{
	if (myActor)
	{

		ATopDownShooteUE4Character* myCharacter = Cast<ATopDownShooteUE4Character>(myActor);
		if (myCharacter)
		{
			//Creating BlurEffect
			if (BlurPostProcessMaterial)
			{

				UPostProcessComponent* PostProcessComponent = myCharacter->FindComponentByClass<UPostProcessComponent>();
				if (PostProcessComponent)
				{
					PostProcessComponent->AddOrUpdateBlendable(BlurPostProcessMaterial, 1.0f);
				}
			}
		}
	}
}

void UTPS_StateEffect_Stun::PlayStunAnimation()
{
	UE_LOG(LogTemp, Error, TEXT("PLAY ANIM TPSEFFECT_STUN"))
	if (myActor)
	{
		ATopDownShooteUE4Character* myCharacter = Cast<ATopDownShooteUE4Character>(myActor);
		if (myCharacter && StunAnimation)
		{
			UAnimInstance* AnimInstance = myCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(StunAnimation, AnimationPlayRate);
			}
		}
	}
}

void UTPS_StateEffect_Stun::StopStunAnimation()
{
	UE_LOG(LogTemp, Error, TEXT("STOP ANIM TPSEFFECT_STUN"))
	if (myActor)
	{
		ATopDownShooteUE4Character* myCharacter = Cast<ATopDownShooteUE4Character>(myActor);
		if (myCharacter && StunAnimation)
		{
			UAnimInstance* AnimInstance = myCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Stop(0.2f, StunAnimation); 
			}
		}
	}
}

