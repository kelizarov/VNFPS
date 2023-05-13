// Fill out your copyright notice in the Description page of Project Settings.

#include "VNBasePaperCharacter.h"
#include "PaperFlipbook.h"
#include "VNCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"

AVNBasePaperCharacter::AVNBasePaperCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UVNCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	MySprite = GetSprite();
	MySprite->SetUsingAbsoluteRotation(true);
	MySprite->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bIsPlayingAnimation = false;
	CurrentAnimation = nullptr;

	Health = 100;

	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.0f;
}

float AVNBasePaperCharacter::GetHealth() const
{
	return Health;
}

void AVNBasePaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetermineDirection();
	DetermineAnimationState();
	if (!bIsDying && !bIsPlayingAnimation && bIsMoving != bWasMoving)
	{
		if (bIsMoving && !bWasMoving)
			PlayAnimation(&MovingAnimation, true);
		else if (!bIsMoving && bWasMoving)
			PlayAnimation(&IdleAnimation, true);
	}
	bWasMoving = bIsMoving;
}

void AVNBasePaperCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateAnimation();
}

float AVNBasePaperCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<AVNBasePaperCharacter>()->Health;
}

ECharacterDirection AVNBasePaperCharacter::GetCharacterDirection() const
{
	return CurrentDirection;
}

void AVNBasePaperCharacter::SetCharacterDirection(ECharacterDirection NewDirection)
{
	ECharacterDirection LastDirection = CurrentDirection;

	CurrentDirection = NewDirection;

	if (CurrentDirection != LastDirection)
		OnChangedDirection();
}

bool AVNBasePaperCharacter::IsAlive() const
{
	return Health > 0;
}

void AVNBasePaperCharacter::DetermineDirection()
{
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector ActorLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector DirectionToPlayer = PlayerLocation - ActorLocation;

	ECharacterDirection Direction = ECharacterDirection::Unknown;
	int32 Side = (int32)FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightVector, DirectionToPlayer.GetSafeNormal()))) / 30;
	if (FVector::DotProduct(ForwardVector, DirectionToPlayer) > 0)
	{
		switch (Side)
		{
		case 0:
			Direction = ECharacterDirection::Left;
			break;
		case 1:
			Direction = ECharacterDirection::FrontLeft;
			break;
		case 2: case 3:
			Direction = ECharacterDirection::Front;
			break;
		case 4:
			Direction = ECharacterDirection::FrontRight;
			break;
		case 5:
			Direction = ECharacterDirection::Right;
			break;
		}
	}
	else
	{
		switch (Side)
		{
		case 0:
			Direction = ECharacterDirection::Left;
			break;
		case 1:
			Direction = ECharacterDirection::BackLeft;
			break;
		case 2: case 3:
			Direction = ECharacterDirection::Back;
			break;
		case 4:
			Direction = ECharacterDirection::BackRight;
			break;
		case 5:
			Direction = ECharacterDirection::Right;
			break;
		}
	}
	SetCharacterDirection(Direction);
}

void AVNBasePaperCharacter::UpdateAnimation()
{
	if (bIsDying)
		return;
	if (bIsMoving)
	{
		PlayAnimation(&MovingAnimation, true);
	}
	else
	{
		PlayAnimation(&IdleAnimation, true);
	}
}

void AVNBasePaperCharacter::OnChangedDirection()
{
	if (!bIsDying)
	{
		if (!bIsPlayingAnimation)
			UpdateAnimation();
		else
			PlayAnimation(CurrentAnimation, MySprite->IsLooping(), MySprite->GetPlaybackPosition());
	}
}

void AVNBasePaperCharacter::OnFinishedAnimation()
{
	if (bIsDying)
		return;
	bIsPlayingAnimation = false;
	CurrentAnimation = nullptr;
	UpdateAnimation();
}

void AVNBasePaperCharacter::StopAnimation()
{
	bIsPlayingAnimation = false;
	MySprite->Stop();
	CurrentAnimation = nullptr;
}

void AVNBasePaperCharacter::PlayAnimation(TMap<ECharacterDirection, UPaperFlipbook*>* AnimationToPlay, bool bIsLooping, float PlaybackPosition)
{
	if (!AnimationToPlay || !AnimationToPlay->Contains(CurrentDirection))
		return;
	CurrentAnimation = AnimationToPlay;
	UPaperFlipbook* SpriteToPlay = CurrentAnimation->FindRef(CurrentDirection);
	if (SpriteToPlay == nullptr)
		return;
	if (MySprite->bHiddenInGame)
		MySprite->SetHiddenInGame(false);
	MySprite->SetLooping(bIsLooping);
	MySprite->SetFlipbook(SpriteToPlay);
	MySprite->SetPlaybackPosition(PlaybackPosition, false);
	if (!bIsLooping)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_FinishedAnimation);
		GetWorldTimerManager().SetTimer(TimerHandle_FinishedAnimation, this, &AVNBasePaperCharacter::OnFinishedAnimation, SpriteToPlay->GetTotalDuration(), false);
	}
	MySprite->Play();
}

void AVNBasePaperCharacter::PlayAnimation(UPaperFlipbook* AnimationToPlay, bool bIsLooping, float PlaybackPosition)
{
	if (!AnimationToPlay || AnimationToPlay == nullptr)
		return;
	if (MySprite->bHiddenInGame)
		MySprite->SetHiddenInGame(false);
	MySprite->SetLooping(bIsLooping);
	MySprite->SetFlipbook(AnimationToPlay);
	MySprite->SetPlaybackPosition(PlaybackPosition, false);
	if (!bIsLooping)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_FinishedAnimation);
		GetWorldTimerManager().SetTimer(TimerHandle_FinishedAnimation, this, &AVNBasePaperCharacter::OnFinishedAnimation, AnimationToPlay->GetTotalDuration(), false);
	}
	MySprite->Play();
}

void AVNBasePaperCharacter::DetermineAnimationState()
{
	bIsInAir = GetMovementComponent()->IsFalling();
	bIsMoving = FMath::Abs(GetVelocity().Size()) > 0 ? true : false;
}

void AVNBasePaperCharacter::PlayAttackAnimation()
{
	if (bIsDying)
		return;
	if (bIsPlayingAnimation)
		StopAnimation();
	bIsPlayingAnimation = true;
	PlayAnimation(&AttackAnimation);
}

void AVNBasePaperCharacter::PlayDeathAnimation()
{
	if (bIsDying)
		return;
	if (bIsPlayingAnimation)
		StopAnimation();
	bIsPlayingAnimation = true;
	PlayAnimation(DeathAnimation);
}

void AVNBasePaperCharacter::PlayStaggerAnimation()
{
	if (bIsDying)
		return;
	if (bIsPlayingAnimation)
		StopAnimation();
	bIsPlayingAnimation = true;
	PlayAnimation(&StaggerAnimation);
}

float AVNBasePaperCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health < 0.f)
		return 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			bool bCanDie = true;

			if (DamageEvent.DamageTypeClass)
			{
				// USDamageType* DmgType = Cast<USDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
				// bCanDie = (DmgType == nullptr || (DmgType && DmgType->CanDieFrom()));
			}
			if (bCanDie)
			{
				Die(Damage, DamageEvent, EventInstigator, DamageCauser);
			}
			else
			{
				Health = 1.0f;
			}
		}
		else
		{
			APawn* Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(Damage, DamageEvent, Pawn, DamageCauser, false);
		}
	}
	return ActualDamage;
}

bool AVNBasePaperCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying ||
		IsPendingKill())
		return false;
	return true;
}

void AVNBasePaperCharacter::FellOutOfWorld(const class UDamageType& DmgType)
{
	Die(Health, FDamageEvent(DmgType.GetClass()), NULL, NULL);
}

bool AVNBasePaperCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
		return false;

	Health = FMath::Min(0.0f, Health);
	
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* KilledPlayer = Controller ? Controller : Cast<AController>(GetOwner());
	// GetWorld()->GetAuthGameMode<AVNGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void AVNBasePaperCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	if (bIsDying)
		return;

	PlayHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

	bIsDying = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AVNBasePaperCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	if (bKilled && DeathSound)
	{
		PlayDeathAnimation();
		UGameplayStatics::SpawnSoundAttached(DeathSound, RootComponent, NAME_None, GetActorLocation(), EAttachLocation::SnapToTarget, true);
	}
	else if (HitSound)
	{
		PlayStaggerAnimation();
		UGameplayStatics::SpawnSoundAttached(HitSound, RootComponent, NAME_None, GetActorLocation(), EAttachLocation::SnapToTarget, true);
	}
}

FRotator AVNBasePaperCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void AVNBasePaperCharacter::Kill()
{
	Die(Health, FDamageEvent(NULL), NULL, NULL);
}

void AVNBasePaperCharacter::GetHit(float Damage)
{
	TakeDamage(Damage, FDamageEvent(NULL), GetWorld()->GetFirstPlayerController(), this);
}

void AVNBasePaperCharacter::SetSprinting(bool bNewSprinting, bool bToggle)
{
	bWantsToRun = bNewSprinting;
	bWantsToRunToggle = bNewSprinting && bToggle;

	if (bIsCrouched)
	{
		UnCrouch();
	}
}

bool AVNBasePaperCharacter::IsSprinting() const
{
	if (!GetCharacterMovement())
	{
		return false;
	}
	return (bWantsToRun || bWantsToRunToggle) && !IsTargeting() && !GetVelocity().IsZero()
		&& (FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.1);
}

void AVNBasePaperCharacter::SetTargeting(bool bNewTargeting)
{
	bIsTargeting = bNewTargeting;
}