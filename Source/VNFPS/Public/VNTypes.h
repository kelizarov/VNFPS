// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 *	Default types
 */
#define COLLISION_WEAPON ECC_GameTraceChannel1
#define COLLISION_PROJECTILE ECC_GameTraceChannel2
#define COLLISION_PICKUP ECC_GameTraceChannel3

UENUM(BlueprintType)
enum class ECharacterDirection : uint8 {
  Unknown = 0,
  Back,
  BackLeft,
  BackRight,
  Front,
  FrontLeft,
  FrontRight,
  Left,
  Right
};

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
  Pistol,
  Shotgun,
  Rifle,
  DoubleBarrel,
  GrenadeLauncher,
  PlasmaRifle,
  SniperRifle,
  MachineGun
};

UENUM(BlueprintType)
enum class EWeaponAmmoType : uint8 {
  Bullet,
  Shell,
  Grenade,
  Plasma,
  SniperBullet
};
