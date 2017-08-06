// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VNProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS(Abstract, Blueprintable)
class VNFPS_API AVNProjectile : public AActor
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;


	UFUNCTION()
		void OnImpact(const FHitResult& Impact);

private:
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
		UProjectileMovementComponent* MoveComp;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
		USphereComponent* CollisionComp;

protected:

	TWeakObjectPtr<AController> MyController;

	bool bExploded;

	void Explode(const FHitResult& Impact);

	void DisableAndDestroy();
	
public:	
	// Sets default values for this actor's properties
	AVNProjectile(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		float RadialDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		float RadialRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<UDamageType> DamageType;

	void InitVelocity(const FVector& ShootDir);


};
