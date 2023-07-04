// Fill out your copyright notice in the Description page of Project Settings.

#include "VNWeaponProjectile.h"
#include <Kismet/GameplayStatics.h>

void AVNWeaponProjectile::FireWeapon() {
  FVector ShootDir = GetAdjustedAim();
  FVector Origin = GetMuzzleLocation();
  const FVector StartPos = GetCameraDamageStartLocation(ShootDir);
  const FVector StartTrace = StartPos;
  const float Range = 10000.f;
  const FVector EndTrace = StartTrace + (ShootDir * Range);

  FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
  if (Impact.bBlockingHit) {
    const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
    ShootDir = AdjustedDir;
  }
  SpawnProjectile(Origin, ShootDir);
}

void AVNWeaponProjectile::SpawnProjectile(const FVector &Origin,
                                          const FVector &ShootDir) {
  FTransform SpawnTM(ShootDir.Rotation(), Origin);
  DrawDebugLine(GetWorld(), Origin, Origin + (ShootDir * 100.f), FColor::Red,
                false, 2.f);
  AVNProjectile *Projectile =
      Cast<AVNProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
          this, ProjectileActor, SpawnTM));
  if (Projectile) {
    Projectile->SetInstigator(GetInstigator());
    Projectile->SetOwner(this);
    Projectile->InitVelocity(ShootDir);

    UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
  }
}
