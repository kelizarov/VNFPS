// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VNBasePaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraComponent.h"
#include "VNPlayerPaperCharacter.generated.h"

UCLASS()
class VNFPS_API AVNPlayerPaperCharacter : public AVNBasePaperCharacter
{
	GENERATED_BODY()
	
	AVNPlayerPaperCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PawnClientRestart() override;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		UPaperFlipbookComponent* WeaponSprite;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		UCameraComponent* CameraComp;

public:
	FORCEINLINE UCameraComponent* GetCamera() const
	{
		return CameraComp;
	}

	FORCEINLINE UPaperFlipbookComponent* GetWeaponSprite() const
	{
		return WeaponSprite;
	}
	
	/* Movement */
	virtual void MoveForward(float Val);

	virtual void MoveRight(float Val);

	/* Crouching & Sliding */
	bool bIsSliding;

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void OnCrouchToggle();

	UFUNCTION(BlueprintCallable, Category = "DEBUG | Movement")
		void SetSliding(bool bNewSliding);

	UFUNCTION(BlueprintCallable, Category = "Movement")
		bool IsSliding() const;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
		float SlideSpeedModifier;

	/* Sprinting */
	void OnStartSprinting();

	void OnStartSprintingToggle();

	void OnStopSprinting();

	virtual void SetSprinting(bool bNewSprinting, bool bToggle) override;

	/* Jumping & Double Jumping */
	void OnJump();

	void DoubleJump();

	bool bIsJumping;

	bool bCanDoubleJump;

	UFUNCTION(BlueprintCallable, Category = "Movement")
		bool IsInitiatedJump() const;

	void SetIsJumping(bool bNewJumping);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	/* Targeting */
	void OnStartTargeting();

	void OnStopTargeting();

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
		float NormalFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
		float TargetingFOV;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
		float GetNormalFOV() const { return NormalFOV; }

	UFUNCTION(BlueprintCallable, Category = "Targeting")
		float GetTargetingFOV() const { return TargetingFOV; }

	/* Armor */
	float Armor;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetArmor() const { return Armor; }

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetMaxArmor() const { return GetClass()->GetDefaultObject<AVNPlayerPaperCharacter>()->Armor; }

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		bool HasArmor() const { return Armor > 0; }

	/* Weapon & Inventory */
	bool bWantsToFire;

	void OnReload();

	void OnStartFire();

	void OnStopFire();

	void StartWeaponFire();

	void StopWeaponFire();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TMap<EWeaponAmmoType, int32> InventoryAmmo;

public:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		int32 GetAmmoByType(EWeaponAmmoType AmmoType) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		int32 GetMaxAmmoByType(EWeaponAmmoType AmmoType) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		AVNWeapon* GetCurrentWeapon() const;

	bool CanFire() const;

	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool IsFiring() const;

	TArray<AVNWeapon*> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TArray<TSubclassOf<class AVNWeapon>> DefaultInventory;

	void SetCurrentWeapon(class AVNWeapon* NewWeapon, class AVNWeapon* LastWeapon = nullptr);

	void EquipWeapon(AVNWeapon* Weapon);

	void AddWeapon(class AVNWeapon* Weapon);

	void DestroyInventory();

	void RemoveWeapon(class AVNWeapon* Weapon, bool bDestroy);

	class AVNWeapon* CurrentWeapon;

	class AVNWeapon* PreviousWeapon;

	void SpawnDefaultInventory();

	void OnEquipPistol();

	void OnEquipShotgun();

	void OnEquipRifle();

	void OnEquipDoubleBarrel();

	void OnEquipGrenadeLauncher();

	void OnEquipPlasmaRifle();

	void OnEquipFirst();

	void OnEquipSecond();

	void OnEquipThird();

	void OnEquipFourth();
	
};
