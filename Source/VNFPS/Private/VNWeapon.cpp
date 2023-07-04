// Fill out your copyright notice in the Description page of Project Settings.

#include "VNWeapon.h"
#include "PaperFlipbookComponent.h"
#include "VNPlayerController.h"
#include "VNPlayerPaperCharacter.h"

AVNWeapon::AVNWeapon(const class FObjectInitializer &ObjectInitializer) {
  bIsEquipped = false;
  CurrentState = EWeaponState::Idle;

  PrimaryActorTick.bCanEverTick = true;

  ShotsPerMinute = 700;
  MaxAmmoPerClip = 30;
  NoAnimReloadDuration = 1.5f;
  NoAnimEquipDuration = 0.5f;
  NmbOfShots = 1;
}

void AVNWeapon::PostInitializeComponents() {
  Super::PostInitializeComponents();

  // TimeBetweenShots = 60.0f / ShotsPerMinute;
  TimeBetweenShots = FiringAnimation != nullptr
                         ? FiringAnimation->GetTotalDuration()
                         : 60.0f / ShotsPerMinute;
  CurrentAmmo = 0;
  CurrentAmmoInClip = MaxAmmoPerClip;
  ReloadDuration = ReloadAnimation != nullptr
                       ? ReloadAnimation->GetTotalDuration()
                       : NoAnimReloadDuration;
}

void AVNWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Super::EndPlay(EndPlayReason);
}

class AVNPlayerPaperCharacter *AVNWeapon::GetPawnOwner() const {
  return MyPawn;
}

void AVNWeapon::SetOwningPawn(class AVNPlayerPaperCharacter *NewOwner) {
  if (MyPawn != NewOwner) {
    SetInstigator(NewOwner);
    MyPawn = NewOwner;
    SetOwner(NewOwner);
  }
}

void AVNWeapon::OnEquip(bool bPlayAnimation) {
  bPendingEquip = true;
  DetermineWeaponState();

  if (bPlayAnimation) {
    float Duration = PlayAnimation(EquippingAnimation);
    if (Duration <= 0.f) {
      EquipDuration = NoAnimEquipDuration;
    }
    EquipStartedTime = GetWorld()->TimeSeconds;

    GetWorldTimerManager().SetTimer(TimerHandle_EquipFinished, this,
                                    &AVNWeapon::OnEquipFinished, Duration,
                                    false);
  } else {
    OnEquipFinished();
  }
}

void AVNWeapon::OnUnEquip() {
  bIsEquipped = false;
  StopFire();

  if (bPendingEquip) {
    bPendingEquip = false;
    GetWorldTimerManager().ClearTimer(TimerHandle_EquipFinished);
  }
  if (bPendingReload) {
    bPendingReload = false;
    GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
  }
  DetermineWeaponState();
}

void AVNWeapon::OnEnterInventory(class AVNPlayerPaperCharacter *NewOwner) {
  SetOwningPawn(NewOwner);
}

void AVNWeapon::OnLeaveInventory() {
  SetOwningPawn(nullptr);
  OnUnEquip();
}

bool AVNWeapon::IsEquipped() const { return bIsEquipped; }

void AVNWeapon::StartFire() {
  if (!bWantsToFire) {
    bWantsToFire = true;
    DetermineWeaponState();
  }
}

void AVNWeapon::StopFire() {
  if (bWantsToFire) {
    bWantsToFire = false;
    DetermineWeaponState();
  }
}

bool AVNWeapon::CanFire() const {
  bool bPawnCanFire = MyPawn && MyPawn->CanFire();
  bool StateOK = CurrentState == EWeaponState::Idle ||
                 CurrentState == EWeaponState::Firing;

  return bPawnCanFire && StateOK && !bPendingReload;
}

FVector AVNWeapon::GetAdjustedAim() const {
  const AVNPlayerController *PC =
      GetInstigator() ? Cast<AVNPlayerController>(GetInstigator()->Controller)
                      : nullptr;
  FVector FinalAim = FVector::ZeroVector;

  if (PC) {
    FVector CamLoc;
    FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    FinalAim = CamRot.Vector();
  } else if (GetInstigator()) {
    FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
  }

  return FinalAim;
}

FVector AVNWeapon::GetCameraDamageStartLocation(const FVector &AimDir) const {
  const AVNPlayerController *PC =
      MyPawn ? Cast<AVNPlayerController>(MyPawn->Controller) : nullptr;
  FVector OutStartTrace = FVector::ZeroVector;

  if (PC) {
    FRotator DummyRot;
    PC->GetPlayerViewPoint(OutStartTrace, DummyRot);

    OutStartTrace =
        OutStartTrace +
        AimDir *
            (FVector::DotProduct(
                GetInstigator()->GetActorLocation() - OutStartTrace, AimDir));
  }

  return OutStartTrace;
}

FVector AVNWeapon::GetMuzzleLocation() const {
  const AVNPlayerController *PC =
      MyPawn ? Cast<AVNPlayerController>(MyPawn->Controller) : nullptr;
  FVector OutLocation = FVector::ZeroVector;

  if (PC) {
    FRotator DummyRot;
    PC->GetPlayerViewPoint(OutLocation, DummyRot);
  }
  return OutLocation;
}

FHitResult AVNWeapon::WeaponTrace(const FVector &TraceFrom,
                                  const FVector &TraceTo) const {
  FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, GetInstigator());
  // TraceParams.bTraceAsyncScene = true;
  TraceParams.bReturnPhysicalMaterial = true;

  FHitResult Hit(ForceInit);
  GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo,
                                       COLLISION_WEAPON, TraceParams);
  // DrawDebugLine(GetWorld(), TraceFrom, TraceTo, FColor::Red, false, 1.0f);

  return Hit;
}

void AVNWeapon::HandleFiring() {
  if (CurrentAmmoInClip > 0.f && CanFire()) {
    if (MyPawn) {
      for (int32 i = 0; i < NmbOfShots; i++)
        FireWeapon();

      UseAmmo();

      SimulateWeaponFire();
    }
  } else if (CanReload()) {
    StartReload();
  }

  if (MyPawn) {
    bRefiring =
        (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.f);
    if (bCanRefire && bRefiring) {
      GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this,
                                      &AVNWeapon::HandleFiring,
                                      TimeBetweenShots, false);
    }
  }

  LastFireTime = GetWorld()->TimeSeconds;
}

void AVNWeapon::SimulateWeaponFire() {
  PlayAnimation(FiringAnimation);
  MyPawn->PlayAnimation(&MyPawn->AttackAnimation);
  GetWorldTimerManager().SetTimer(TimerHandle_StopSimulatingFire, this,
                                  &AVNWeapon::StopSimulatingWeaponFire,
                                  FiringAnimation->GetTotalDuration(), false);
}

void AVNWeapon::StopSimulatingWeaponFire() {
  if (IsEquipped()) {
    if (MyPawn->IsSprinting())
      PlaySprintAnimation();
    else
      PlayAnimation(IdleAnimation, true);
  }
}

EWeaponState AVNWeapon::GetCurrentState() const { return CurrentState; }

void AVNWeapon::SetWeaponState(EWeaponState NewState) {
  const EWeaponState PrevState = CurrentState;

  if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing) {
    OnBurstFinished();
  }

  CurrentState = NewState;

  if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing) {
    OnBurstStarted();
  }
}

void AVNWeapon::OnBurstStarted() {
  const float GameTime = GetWorld()->TimeSeconds;
  if (LastFireTime > 0 && TimeBetweenShots > 0.f &&
      LastFireTime + TimeBetweenShots > GameTime) {
    GetWorldTimerManager().SetTimer(
        TimerHandle_HandleFiring, this, &AVNWeapon::HandleFiring,
        LastFireTime + TimeBetweenShots - GameTime, false);
  } else {
    HandleFiring();
  }
}
void AVNWeapon::OnBurstFinished() {
  GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
  bRefiring = false;
}

void AVNWeapon::DetermineWeaponState() {
  EWeaponState NewState = EWeaponState::Idle;

  if (bIsEquipped) {
    if (bPendingReload) {
      if (CanReload()) {
        NewState = EWeaponState::Reloading;
      } else {
        NewState = CurrentState;
      }
    } else if (!bPendingReload && bWantsToFire && CanFire()) {
      NewState = EWeaponState::Firing;
    }
  } else if (bPendingEquip) {
    NewState = EWeaponState::Equipping;
  }
  SetWeaponState(NewState);
}

float AVNWeapon::GetEquipStartedTime() const { return EquipStartedTime; }

float AVNWeapon::GetEquipDuration() const { return EquipDuration; }

void AVNWeapon::OnEquipFinished() {
  bIsEquipped = true;
  bPendingEquip = false;

  DetermineWeaponState();

  PlayAnimation(IdleAnimation, true);

  if (MyPawn) {
    if (CurrentAmmoInClip <= 0 && CanReload()) {
      StartReload();
    }
    if (MyPawn->IsSprinting()) {
      PlaySprintAnimation();
    }
    if (bWantsToFire) {
      DetermineWeaponState();
    }
  }
}

void AVNWeapon::UseAmmo() { CurrentAmmoInClip--; }

int32 AVNWeapon::GiveAmmo(int32 AddAmount) {
  if (!MyPawn)
    return 0;
  const int32 MissingAmmo = FMath::Max(0, GetMaxAmmo() - GetCurrentAmmo());
  AddAmount = FMath::Min(AddAmount, MissingAmmo);
  MyPawn->InventoryAmmo[AmmoType] += AddAmount;

  return FMath::Max(0, AddAmount - MissingAmmo);
}

void AVNWeapon::SetAmmoCount(int32 AddAmount) {
  if (!MyPawn)
    return;
  MyPawn->InventoryAmmo[AmmoType] = FMath::Min(GetMaxAmmo(), AddAmount);
  CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, GetCurrentAmmo());
}

int32 AVNWeapon::GetCurrentAmmo() const {
  if (!MyPawn)
    return 0;
  return MyPawn->GetAmmoByType(AmmoType);
}

int32 AVNWeapon::GetCurrentAmmoInClip() const { return CurrentAmmoInClip; }

int32 AVNWeapon::GetMaxAmmo() const {
  if (!MyPawn)
    return 0;
  return MyPawn->GetMaxAmmoByType(AmmoType);
}

int32 AVNWeapon::GetMaxAmmoPerClip() const { return MaxAmmoPerClip; }

void AVNWeapon::StartReload() {
  if (CanReload()) {
    bPendingReload = true;
    DetermineWeaponState();
    GetWorldTimerManager().ClearTimer(TimerHandle_StopSimulatingFire);

    if (bLoadPerBullet) {
      LoadBullet();
    } else {
      PlayAnimation(ReloadAnimation);
      GetWorldTimerManager().SetTimer(
          TimerHandle_ReloadWeapon, this, &AVNWeapon::ReloadWeapon,
          FMath::Max(0.1f, ReloadDuration - 0.1f), false);
      GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this,
                                      &AVNWeapon::StopSimulateReload,
                                      ReloadDuration, false);
    }
  }
}

void AVNWeapon::LoadBullet() {
  if (GetCurrentAmmoInClip() < GetMaxAmmoPerClip()) {
    PlayAnimation(ReloadAnimation);
    GetWorldTimerManager().SetTimer(
        TimerHandle_ReloadWeapon, this, &AVNWeapon::ReloadWeapon,
        FMath::Max(0.1f, ReloadDuration - 0.1f), false);
  } else {
    StopSimulateReload();
  }
}

void AVNWeapon::StopSimulateReload() {
  if (CurrentState == EWeaponState::Reloading) {
    bPendingReload = false;
    DetermineWeaponState();
    if (IsEquipped()) {
      if (MyPawn->IsSprinting())
        PlaySprintAnimation();
      else
        PlayAnimation(IdleAnimation, true);
    }
  }
}

void AVNWeapon::ReloadWeapon() {
  if (bLoadPerBullet) {
    if (GetCurrentAmmoInClip() < GetMaxAmmoPerClip()) {
      CurrentAmmoInClip += 1;
      MyPawn->InventoryAmmo[AmmoType] -= 1;
      LoadBullet();
    }
  } else {
    int32 ClipDelta = FMath::Min(GetMaxAmmoPerClip() - GetCurrentAmmoInClip(),
                                 GetCurrentAmmo() - GetCurrentAmmoInClip());

    if (ClipDelta > 0) {
      CurrentAmmoInClip += ClipDelta;
      MyPawn->InventoryAmmo[AmmoType] -= ClipDelta;
    }
  }
}

bool AVNWeapon::CanReload() const {
  bool bCanReload = MyPawn && MyPawn->CanReload();
  bool bHasAmmo = (GetCurrentAmmoInClip() < GetMaxAmmoPerClip()) &&
                  ((GetCurrentAmmo() - GetCurrentAmmoInClip()) > 0);
  bool bStateOK = ((CurrentState == EWeaponState::Idle) ||
                   (CurrentState == EWeaponState::Firing));

  return bCanReload && bHasAmmo && bStateOK;
}

float AVNWeapon::PlayAnimation(UPaperFlipbook *AnimationToPlay, bool bIsLooping,
                               float PlaybackPosition) {
  if (!AnimationToPlay && AnimationToPlay == nullptr && !MyPawn)
    return 0.f;
  UPaperFlipbookComponent *Sprite = MyPawn->GetWeaponSprite();
  if (Sprite->bHiddenInGame)
    Sprite->SetHiddenInGame(false);
  Sprite->SetLooping(bIsLooping);
  Sprite->SetFlipbook(AnimationToPlay);
  Sprite->SetPlaybackPosition(PlaybackPosition, false);
  Sprite->Play();

  return AnimationToPlay->GetTotalDuration();
}

void AVNWeapon::StopAnimation(UPaperFlipbook *Animation) {
  if (Animation && Animation == MyPawn->GetWeaponSprite()->GetFlipbook()) {
    MyPawn->GetWeaponSprite()->Stop();
  }
}

void AVNWeapon::PlaySprintAnimation() {
  if (!bPendingReload && !bPendingEquip && !bWantsToFire)
    PlayAnimation(SprintAnimation, true);
}

void AVNWeapon::StopSprintAnimation() {
  if (IsEquipped()) {
    if (!bPendingReload && !bPendingEquip && !bWantsToFire)
      PlayAnimation(IdleAnimation, true);
  }
}
