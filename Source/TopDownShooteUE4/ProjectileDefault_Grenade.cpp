// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault_Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void AProjectileDefault_Grenade::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplose(DeltaTime);


}

void AProjectileDefault_Grenade::TimerExplose(float DeltaTime)
{
	if (TimerEnabled)
	{
		if (TimerToExplose > TimeToExplose)
		{
			//Explose
			Explose();

		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AProjectileDefault_Grenade::BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileDefault_Grenade::ImpactProjectile()
{
	//Init Grenade
	TimerEnabled = true;
}

void AProjectileDefault_Grenade::Explose()
{
	if (DebugExplodeShow)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMinRadiusDamage, 12, FColor::Green, false, 12.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMaxRadiusDamage, 12, FColor::Green, false, 12.0f);
	}


	TimerEnabled = false;
	if (ProjectileSetting.ExploseFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSetting.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (ProjectileSetting.ExploseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.ExploseSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExploseMaxDamage,
		ProjectileSetting.ExploseMaxDamage * 0.2f,
		GetActorLocation(),
		ProjectileSetting.ProjectileMinRadiusDamage,
		ProjectileSetting.ProjectileMaxRadiusDamage,
		5,
		NULL, IgnoredActor, this, nullptr);

	this->Destroy();
}

