// Fill out your copyright notice in the Description page of Project Settings.

#include "VNFPS.h"
#include "VNWeaponInstant.h"

AVNWeaponInstant::AVNWeaponInstant(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HitDamage = 26;
	WeaponRange = 15000;

	AllowedViewDotHitDir = -1.0f;
	ClientSideHitLeeway = 200.0f;
	DefaultSpreadHorizontal = 1.f;
	DefaultSpreadVertical = 1.f;
}

void AVNWeaponInstant::FireWeapon()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponStream(RandomSeed);

	const float CurrentSpreadHorizontal = DefaultSpreadHorizontal;
	const float CurrentSpreadVertical = DefaultSpreadVertical;
	const float ConeHalfAngleHorizontal = FMath::DegreesToRadians(CurrentSpreadHorizontal * 0.5f);
	const float ConeHalfAngleVertical = FMath::DegreesToRadians(CurrentSpreadVertical * 0.5f);
	const FVector AimDir = GetAdjustedAim();
	const FVector ShootDir = WeaponStream.VRandCone(AimDir, ConeHalfAngleHorizontal, ConeHalfAngleVertical);
	const FVector CameraPos = GetCameraDamageStartLocation(AimDir);
	const FVector EndPos = CameraPos + (ShootDir * WeaponRange);

	FHitResult Impact = WeaponTrace(CameraPos, EndPos);
	if (Impact.bBlockingHit)
		DealDamage(Impact, ShootDir);
}

void AVNWeaponInstant::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	float ActualDamage = HitDamage;

	FPointDamageEvent PointDmg;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = ActualDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}