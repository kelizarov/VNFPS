// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <PaperCharacter.h>
#include <PaperFlipbookComponent.h>
#include "VNTypes.h"
#include "Sound/SoundCue.h"
#include "VNBasePaperCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VNFPS_API AVNBasePaperCharacter : public APaperCharacter
{
	GENERATED_BODY()

	ECharacterDirection CurrentDirection;

	void DetermineDirection();

	void SetCharacterDirection(ECharacterDirection NewDirection);

public:

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		TMap<ECharacterDirection, UPaperFlipbook*> IdleAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		TMap<ECharacterDirection, UPaperFlipbook*> MovingAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		TMap<ECharacterDirection, UPaperFlipbook*> AttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		TMap<ECharacterDirection, UPaperFlipbook*> StaggerAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UPaperFlipbook* DeathAnimation;

	TMap<ECharacterDirection, UPaperFlipbook*>* CurrentAnimation;

	void UpdateAnimation();

	void PlayAnimation(TMap<ECharacterDirection, UPaperFlipbook*>* AnimationToPlay, bool bIsLooping = false, float PlaybackPosition = 0.f);

	void PlayAnimation(UPaperFlipbook* AnimationToPlay, bool bIsLooping = false, float CurrentPlayTime = 0.f);

	void StopAnimation();

	UFUNCTION(BlueprintCallable, Category = Animation)
		void OnFinishedAnimation();

	void DetermineAnimationState();

	void OnChangedDirection();


	bool bIsPlayingAnimation;

	bool bIsMoving;

	bool bIsInAir;

	FTimerHandle TimerHandle_FinishedAnimation;

	bool bWasAttacking;

	bool bWasMoving;

	bool bWasInAir;

	AVNBasePaperCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		bool IsAlive() const;


	UFUNCTION(BlueprintCallable, Category = "Targeting")
		FRotator GetAimOffsets() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterDirection")
		ECharacterDirection GetCharacterDirection() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
		void PlayAttackAnimation();
	UFUNCTION(BlueprintCallable, Category = Animation)
		void PlayDeathAnimation();
	UFUNCTION(BlueprintCallable, Category = Animation)
		void PlayStaggerAnimation();

protected:
	UPaperFlipbookComponent* MySprite;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
		float Health;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	bool bIsDying;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* DeathSound;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* HitSound;

	/* Sprinting */
	UPROPERTY(EditDefaultsOnly, Category = Movement)
		float SprintingSpeedModifier;

	bool bWantsToRun;

	bool bWantsToRunToggle;

	virtual void SetSprinting(bool bNewSprinting, bool bToggle);

	/* Targeting */
	bool bIsTargeting;

	void SetTargeting(bool bNewTargeting);

	
public:
	UFUNCTION(BlueprintCallable, Category = Targeting)
		bool IsTargeting() const { return bIsTargeting; }

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
		float TargetingSpeedModifier;

	UFUNCTION(BlueprintCallable, Category = Movement)
		virtual bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = Movement)
		float GetSprintSpeedModifier() const { return SprintingSpeedModifier; }

	UFUNCTION(BlueprintCallable, Category = Targeting)
		float GetTargetingSpeedModifier() const { return TargetingSpeedModifier; }

	/* DEBUG FUNCTION DELETE LATER */
	UFUNCTION(BlueprintCallable, Category = Debug)
		void Kill();

	UFUNCTION(BlueprintCallable, Category = Debug)
		void GetHit(float Damage = 10.f);
};
