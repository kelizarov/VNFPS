// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VNWeapon.h"
#include "VNProjectile.h"
#include "VNWeaponProjectile.generated.h"

/**
 * 
 */
UCLASS()
class VNFPS_API AVNWeaponProjectile : public AVNWeapon
{
	GENERATED_BODY()

	virtual void FireWeapon() override;

	void SpawnProjectile(const FVector& Origin, const FVector& ShootDir);

public:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AVNProjectile> ProjectileActor;


	
	
};
