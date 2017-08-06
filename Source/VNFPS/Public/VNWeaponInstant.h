// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VNWeapon.h"
#include "VNWeaponInstant.generated.h"

/**
 * 
 */
UCLASS()
class VNFPS_API AVNWeaponInstant : public AVNWeapon
{
	GENERATED_BODY()

	AVNWeaponInstant(const class FObjectInitializer& ObjectInitializer);

	virtual void FireWeapon() override;

	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

private:

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
		float HitDamage;

	UPROPERTY(EditDefaultsOnly)
		float WeaponRange;

	UPROPERTY(EditDefaultsOnly)
		float AllowedViewDotHitDir;

	UPROPERTY(EditDefaultsOnly)
		float ClientSideHitLeeway;

	UPROPERTY(EditDefaultsOnly)
		float DefaultSpreadHorizontal;

	UPROPERTY(EditDefaultsOnly)
		float DefaultSpreadVertical;
	
};
