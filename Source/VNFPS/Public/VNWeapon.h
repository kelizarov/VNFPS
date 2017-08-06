// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VNTypes.h"
#include "VNPlayerPaperCharacter.h"
#include "PaperFlipbook.h"
#include "VNWeapon.generated.h"

UENUM()
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

UCLASS()
class VNFPS_API AVNWeapon : public AActor
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	float GetEquipStartedTime() const;

	float GetEquipDuration() const;

	float EquipStartedTime;

	float EquipDuration;

	bool bIsEquipped;

	bool bPendingEquip;

	FTimerHandle TimerHandle_HandleFiring;

	FTimerHandle TimerHandle_EquipFinished;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float ShotsPerMinute;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		bool bLoadPerBullet;
	
public:	
	// Sets default values for this actor's properties
	AVNWeapon(const class FObjectInitializer& ObjectInitializer);
	

protected:

	class AVNPlayerPaperCharacter* MyPawn;
	
	/* Animation */

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UPaperFlipbook* FiringAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UPaperFlipbook* EquippingAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UPaperFlipbook* ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UPaperFlipbook* SprintAnimation;

	float PlayAnimation(UPaperFlipbook* AnimationToPlay, bool bIsLooping = false, float PlaybackPosition = 0.0f);

	void StopAnimation(UPaperFlipbook* Animation);

	bool bIsPlayingFiringAnimation;

	void SimulateWeaponFire();

	void StopSimulatingWeaponFire();

	FTimerHandle TimerHandle_StopSimulatingFire;

	/* Equipping && Inventory */

	virtual void OnEquipFinished();

	bool IsEquipped() const;

public:

	virtual void OnUnEquip();

	void OnEquip(bool bPlayAnimation = false);

	void SetOwningPawn(class AVNPlayerPaperCharacter* NewOwner);

	UFUNCTION(BlueprintCallable, Category = Weapon)
		class AVNPlayerPaperCharacter* GetPawnOwner() const;

	virtual void OnEnterInventory(class AVNPlayerPaperCharacter* NewOwner);

	virtual void OnLeaveInventory();

	/* Firing && Reloading*/

public:

	void StartFire();

	void StopFire();

	EWeaponState GetCurrentState() const;

protected:

	bool CanFire() const;

	FVector GetAdjustedAim() const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	FVector GetMuzzleLocation() const;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	virtual void FireWeapon() PURE_VIRTUAL(VNWeapon::FireWeapon, );

private:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bCanRefire;

	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	virtual void HandleFiring();

	void OnBurstStarted();

	void OnBurstFinished();

	bool bWantsToFire;

	EWeaponState CurrentState;

	bool bRefiring;

	float LastFireTime;

	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly)
		int32 NmbOfShots;

private:
	
	FTimerHandle TimerHandle_ReloadWeapon;

	FTimerHandle TimerHandle_StopReload;

	FTimerHandle TimerHandle_LoadPerBullet;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		float NoAnimReloadDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		float NoAnimEquipDuration;

	float ReloadDuration;

	bool bPendingReload;

	void UseAmmo();

	int32 CurrentAmmo;

	int32 CurrentAmmoInClip;

	UPROPERTY(EditDefaultsOnly)
		int32 MaxAmmoPerClip;

	virtual void ReloadWeapon();
	
	bool CanReload() const;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		EWeaponType WeaponType;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		EWeaponAmmoType AmmoType;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	EWeaponAmmoType GetAmmoType() const
	{
		return AmmoType;
	}

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	EWeaponType GetWeaponType() const
	{
		return WeaponType;
	}

	virtual void StartReload();

	virtual void StopSimulateReload();

	void LoadBullet();

	int32 GiveAmmo(int32 AddAmount);

	void SetAmmoCount(int32 AddAmount);

	UFUNCTION(BlueprintCallable, Category = Ammo)
		int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = Ammo)
		int32 GetCurrentAmmoInClip() const;

	UFUNCTION(BlueprintCallable, Category = Ammo)
		int32 GetMaxAmmo() const;

	UFUNCTION(BlueprintCallable, Category = Ammo)
		int32 GetMaxAmmoPerClip() const;

	void PlaySprintAnimation();

	void StopSprintAnimation();
};
