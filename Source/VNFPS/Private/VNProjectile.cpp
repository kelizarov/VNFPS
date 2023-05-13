// Fill out your copyright notice in the Description page of Project Settings.

#include "VNProjectile.h"
#include "VNWeaponProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AVNProjectile::AVNProjectile(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collision"));
	CollisionComp->InitSphereRadius(5.f);
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComp;

	MoveComp = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("Movement"));
	MoveComp->UpdatedComponent = CollisionComp;
	MoveComp->InitialSpeed = 2000.f;
	MoveComp->MaxSpeed = 2000.f;
	MoveComp->bRotationFollowsVelocity = true;
	MoveComp->ProjectileGravityScale = 0.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	RadialDamage = 50.f;
	RadialRadius = 100.f;
	DamageType = UDamageType::StaticClass();
}

void AVNProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MoveComp->OnProjectileStop.AddDynamic(this, &AVNProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(GetInstigator());
	AVNWeaponProjectile* OwnerWeapon = Cast<AVNWeaponProjectile>(GetOwner());
	if (OwnerWeapon)
	{
		// Can change some damage data here
	}

	SetLifeSpan(1000.f);
	MyController = GetInstigatorController();
}

void AVNProjectile::InitVelocity(const FVector& ShootDir)
{
	if (MoveComp)
	{
		//MoveComp->Velocity = ShootDir * MoveComp->InitialSpeed;
	}
}

void AVNProjectile::OnImpact(const FHitResult& Impact)
{
	if (!bExploded)
	{
		Explode(Impact);
		DisableAndDestroy();
	}
}

void AVNProjectile::Explode(const FHitResult& Impact)
{
	const FVector NudgeImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.f;

	if (RadialDamage > 0.f && RadialRadius > 0.f)
	{
		UGameplayStatics::ApplyRadialDamage(this, RadialDamage, NudgeImpactLocation, RadialRadius, DamageType, TArray<AActor*>(), this, MyController.Get());
	}

	bExploded = true;
}

void AVNProjectile::DisableAndDestroy()
{
	MoveComp->StopMovementImmediately();

	SetLifeSpan(2.f);
}