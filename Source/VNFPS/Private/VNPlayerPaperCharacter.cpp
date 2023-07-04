// Fill out your copyright notice in the Description page of Project Settings.

#include "VNPlayerPaperCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VNPlayerController.h"
#include "VNWeapon.h"

AVNPlayerPaperCharacter::AVNPlayerPaperCharacter(
    const class FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer) {
  PrimaryActorTick.bCanEverTick = true;

  UCharacterMovementComponent *MoveComp = GetCharacterMovement();
  MoveComp->GravityScale = 1.5f;
  MoveComp->JumpZVelocity = 620;
  MoveComp->bCanWalkOffLedgesWhenCrouching = true;
  MoveComp->MaxWalkSpeedCrouched = 200;
  MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
  MoveComp->MaxAcceleration = 4096.f;
  MoveComp->AirControl = 0.2f;

  CameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(
      this, TEXT("FPSCamera"));
  CameraComp->bUsePawnControlRotation = true;
  CameraComp->SetupAttachment(RootComponent);

  WeaponSprite =
      ObjectInitializer.CreateDefaultSubobject<UPaperFlipbookComponent>(
          this, TEXT("WeaponSprite"));
  WeaponSprite->bOnlyOwnerSee = true;
  WeaponSprite->bOwnerNoSee = false;
  WeaponSprite->AttachToComponent(
      CameraComp, FAttachmentTransformRules::SnapToTargetIncludingScale);

  MySprite->bOwnerNoSee = true;
  MySprite->bOnlyOwnerSee = false;

  Health = 100;
  Armor = 0.f;

  TargetingSpeedModifier = 0.5f;
  SprintingSpeedModifier = 1.5f;
  bCanDoubleJump = true;
  SlideSpeedModifier = 5.0f;

  NormalFOV = 90.f;
  TargetingFOV = 75.f;
}

void AVNPlayerPaperCharacter::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  if (bWantsToRunToggle && !IsSprinting()) {
    SetSprinting(false, false);
  }
  if (!IsSliding()) {
    SetSliding(false);
  }
}

void AVNPlayerPaperCharacter::BeginPlay() {
  Super::BeginPlay();
  SpawnDefaultInventory();
}

void AVNPlayerPaperCharacter::PawnClientRestart() {
  Super::PawnClientRestart();
}

void AVNPlayerPaperCharacter::EndPlay(
    const EEndPlayReason::Type EndPlayReason) {
  Super::EndPlay(EndPlayReason);
}

void AVNPlayerPaperCharacter::SetupPlayerInputComponent(
    class UInputComponent *PlayerInputComponent) {
  check(PlayerInputComponent);
  PlayerInputComponent->BindAxis("MoveForward", this,
                                 &AVNPlayerPaperCharacter::MoveForward);
  PlayerInputComponent->BindAxis("MoveRight", this,
                                 &AVNPlayerPaperCharacter::MoveRight);

  PlayerInputComponent->BindAction("Jump", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnJump);
  PlayerInputComponent->BindAction("Sprint", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnStartSprinting);
  PlayerInputComponent->BindAction("Sprint", IE_Released, this,
                                   &AVNPlayerPaperCharacter::OnStopSprinting);
  PlayerInputComponent->BindAction(
      "SprintToggle", IE_Pressed, this,
      &AVNPlayerPaperCharacter::OnStartSprintingToggle);
  PlayerInputComponent->BindAction("CrouchToggle", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnCrouchToggle);

  PlayerInputComponent->BindAction("Targeting", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnStartTargeting);
  PlayerInputComponent->BindAction("Targeting", IE_Released, this,
                                   &AVNPlayerPaperCharacter::OnStopTargeting);
  PlayerInputComponent->BindAction("Fire", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnStartFire);
  PlayerInputComponent->BindAction("Fire", IE_Released, this,
                                   &AVNPlayerPaperCharacter::OnStopFire);
  PlayerInputComponent->BindAction("Reload", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnReload);

  PlayerInputComponent->BindAction("InventoryPistol", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnEquipPistol);
  PlayerInputComponent->BindAction("InventoryShotgun", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnEquipShotgun);
  PlayerInputComponent->BindAction("InventoryRifle", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnEquipRifle);
  PlayerInputComponent->BindAction(
      "InventoryDoubleBarrel", IE_Pressed, this,
      &AVNPlayerPaperCharacter::OnEquipDoubleBarrel);
  PlayerInputComponent->BindAction(
      "InventoryGrenadeLauncher", IE_Pressed, this,
      &AVNPlayerPaperCharacter::OnEquipGrenadeLauncher);
  PlayerInputComponent->BindAction(
      "InventoryPlasmaRifle", IE_Pressed, this,
      &AVNPlayerPaperCharacter::OnEquipPlasmaRifle);

  PlayerInputComponent->BindAction("InventoryFirst", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnEquipFirst);
  PlayerInputComponent->BindAction("InventorySecond", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnEquipSecond);
  PlayerInputComponent->BindAction("InventoryThird", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnEquipThird);
  PlayerInputComponent->BindAction("InventoryFourth", IE_Pressed, this,
                                   &AVNPlayerPaperCharacter::OnEquipFourth);
}

void AVNPlayerPaperCharacter::MoveForward(float Val) {
  if (Val != 0.f && !bIsDying) {
    const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() ||
                                 GetCharacterMovement()->IsFalling());
    const FRotator Rotation =
        bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
    const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
    Val = bIsSliding ? Val * 0.1 : Val;
    AddMovementInput(Direction, Val);
  }
}

void AVNPlayerPaperCharacter::MoveRight(float Val) {
  if (Val != 0.f && !bIsDying) {
    const FRotator Rotation = GetActorRotation();
    const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
    Val = bIsSliding ? Val * 0.1 : Val;
    AddMovementInput(Direction, Val);
  }
}

void AVNPlayerPaperCharacter::OnJump() { SetIsJumping(true); }

bool AVNPlayerPaperCharacter::IsInitiatedJump() const { return bIsJumping; }

void AVNPlayerPaperCharacter::SetIsJumping(bool bNewJumping) {
  if (bIsCrouched && bNewJumping) {
    UnCrouch();
    if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking) {
      FVector LaunchDirection = GetLastMovementInputVector();
      if (IsSliding()) {
        SetSliding(false);

        LaunchDirection.X *= 1200.f;
        LaunchDirection.Y *= 1200.f;
      }
      LaunchDirection.Z = GetCharacterMovement()->JumpZVelocity;
      LaunchCharacter(LaunchDirection, false, false);
      bIsJumping = true;
    }
  } else if (bNewJumping != bIsJumping) {
    bIsJumping = bNewJumping;
    if (bIsJumping) {
      Jump();
      if (IsSprinting()) {
        SetSprinting(false, false);
      }
    } else {
      bCanDoubleJump = true;
    }
  } else if (GetCharacterMovement()->IsFalling() && bCanDoubleJump) {
    DoubleJump();
  }
}

void AVNPlayerPaperCharacter::DoubleJump() {
  const FVector Velocity = FVector(0, 0, GetCharacterMovement()->JumpZVelocity);
  const bool bXYOverride =
      FVector::DotProduct(GetLastMovementInputVector(), GetVelocity()) < 0;
  LaunchCharacter(Velocity, bXYOverride, true);
  bCanDoubleJump = false;
}

void AVNPlayerPaperCharacter::OnMovementModeChanged(
    EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
  Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
  if (PrevMovementMode == EMovementMode::MOVE_Falling &&
      GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling) {
    SetIsJumping(false);
    if (bIsCrouched) {
      SetSliding(true);
    }
    if (IsSprinting()) {
      if (CurrentWeapon && CurrentWeapon != nullptr)
        CurrentWeapon->PlaySprintAnimation();
    }
  }
}

void AVNPlayerPaperCharacter::OnStartSprinting() { SetSprinting(true, false); }

void AVNPlayerPaperCharacter::OnStopSprinting() { SetSprinting(false, false); }

void AVNPlayerPaperCharacter::OnStartSprintingToggle() {
  SetSprinting(true, true);
}

void AVNPlayerPaperCharacter::OnCrouchToggle() {
  if (IsSprinting()) {
    SetSprinting(false, false);
    SetSliding(true);
  }
  if (CanCrouch()) {
    Crouch();
  } else {
    SetSliding(false);
    UnCrouch();
  }
}

void AVNPlayerPaperCharacter::SetSprinting(bool bNewSprinting, bool bToggle) {
  if ((bNewSprinting || bToggle) && IsTargeting()) {
    SetTargeting(false);
  }
  if ((bNewSprinting || bToggle) && IsSliding()) {
    SetSliding(false);
  }
  Super::SetSprinting(bNewSprinting, bToggle);

  if (CurrentWeapon && CurrentWeapon != nullptr) {
    if (IsSprinting() && GetCharacterMovement()->MovementMode != MOVE_Falling) {
      CurrentWeapon->PlaySprintAnimation();
    } else {
      CurrentWeapon->StopSprintAnimation();
    }
  }
}

void AVNPlayerPaperCharacter::OnStartTargeting() { SetTargeting(true); }

void AVNPlayerPaperCharacter::OnStopTargeting() { SetTargeting(false); }

void AVNPlayerPaperCharacter::SetSliding(bool bNewSliding) {
  UCharacterMovementComponent *MoveComp = GetCharacterMovement();
  if (bIsSliding != bNewSliding) {
    bIsSliding = bNewSliding;
    if (bIsSliding) {
      if (MoveComp->MovementMode == EMovementMode::MOVE_Walking ||
          MoveComp->MovementMode == EMovementMode::MOVE_Falling) {
        if (IsSprinting()) {
          SetSprinting(false, false);
        }
        MoveComp->BrakingFrictionFactor = 0.f;
        MoveComp->GroundFriction = -8.f;
        MoveComp->BrakingDecelerationWalking = 256.f;
      }
    } else {
      MoveComp->BrakingFrictionFactor = 0.5f;
      MoveComp->GroundFriction = 8.f;
      MoveComp->BrakingDecelerationWalking = 2048.f;
    }
  }
}

bool AVNPlayerPaperCharacter::IsSliding() const {
  return bIsSliding &&
         GetVelocity().Size() > GetCharacterMovement()->GetMaxSpeed();
}

bool AVNPlayerPaperCharacter::CanFire() const { return IsAlive(); }

bool AVNPlayerPaperCharacter::CanReload() const {
  return CurrentWeapon && IsAlive();
}

bool AVNPlayerPaperCharacter::IsFiring() const {
  return CurrentWeapon &&
         CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}

void AVNPlayerPaperCharacter::DestroyInventory() {
  for (int32 i = 0; i < Inventory.Num(); i++) {
    AVNWeapon *Weapon = Inventory[i];
    if (Weapon) {
      RemoveWeapon(Weapon, true);
    }
  }
}

void AVNPlayerPaperCharacter::SetCurrentWeapon(class AVNWeapon *NewWeapon,
                                               class AVNWeapon *LastWeapon) {
  PreviousWeapon = LastWeapon;

  AVNWeapon *LocalLastWeapon = nullptr;
  if (LastWeapon) {
    LocalLastWeapon = LastWeapon;
  } else if (NewWeapon != CurrentWeapon) {
    LocalLastWeapon = CurrentWeapon;
  }

  bool bHasPreviousWeapon = false;
  if (LocalLastWeapon) {
    LocalLastWeapon->OnUnEquip();
    bHasPreviousWeapon = true;
  }

  CurrentWeapon = NewWeapon;

  if (NewWeapon) {
    NewWeapon->SetOwningPawn(this);
    NewWeapon->OnEquip(bHasPreviousWeapon);
  }
}

AVNWeapon *AVNPlayerPaperCharacter::GetCurrentWeapon() const {
  return CurrentWeapon;
}

void AVNPlayerPaperCharacter::EquipWeapon(AVNWeapon *Weapon) {
  if (Weapon) {
    if (Weapon == CurrentWeapon)
      return;
    SetCurrentWeapon(Weapon);
  }
}

void AVNPlayerPaperCharacter::AddWeapon(AVNWeapon *Weapon) {
  if (Weapon) {
    Weapon->OnEnterInventory(this);
    Inventory.AddUnique(Weapon);
    if (Inventory.Num() > 0 && CurrentWeapon == nullptr) {
      EquipWeapon(Inventory[0]);
    }
  }
}

void AVNPlayerPaperCharacter::RemoveWeapon(AVNWeapon *Weapon, bool bDestroy) {
  if (Weapon) {
    bool bIsCurrent = CurrentWeapon == Weapon;

    if (Inventory.Contains(Weapon)) {
      Weapon->OnLeaveInventory();
    }
    Inventory.RemoveSingle(Weapon);

    if (bIsCurrent && Inventory.Num() > 0) {
      SetCurrentWeapon(Inventory[0]);
    }

    if (bDestroy) {
      Weapon->Destroy();
    }
  }
}

void AVNPlayerPaperCharacter::OnReload() {
  if (CurrentWeapon) {
    CurrentWeapon->StartReload();
  }
}

void AVNPlayerPaperCharacter::OnStartFire() {
  if (IsSprinting()) {
    SetSprinting(false, false);
  }
  StartWeaponFire();
}

void AVNPlayerPaperCharacter::OnStopFire() { StopWeaponFire(); }

void AVNPlayerPaperCharacter::StartWeaponFire() {
  if (!bWantsToFire) {
    bWantsToFire = true;
    if (CurrentWeapon) {
      CurrentWeapon->StartFire();
    }
  }
}

void AVNPlayerPaperCharacter::StopWeaponFire() {
  if (bWantsToFire) {
    bWantsToFire = false;
    if (CurrentWeapon) {
      CurrentWeapon->StopFire();
    }
  }
}

void AVNPlayerPaperCharacter::SpawnDefaultInventory() {
  for (int32 i = 0; i < DefaultInventory.Num(); i++) {
    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AVNWeapon *Weapon =
        GetWorld()->SpawnActor<AVNWeapon>(DefaultInventory[i], SpawnInfo);
    AddWeapon(Weapon);
  }

  if (Inventory.Num() > 0) {
    EquipWeapon(Inventory[0]);
  }
}

int32 AVNPlayerPaperCharacter::GetAmmoByType(EWeaponAmmoType AmmoType) const {
  if (InventoryAmmo.Num() <= 0 || !InventoryAmmo.Contains(AmmoType))
    return 0;
  return InventoryAmmo[AmmoType];
}

int32 AVNPlayerPaperCharacter::GetMaxAmmoByType(
    EWeaponAmmoType AmmoType) const {
  if (InventoryAmmo.Num() <= 0 || !InventoryAmmo.Contains(AmmoType))
    return 0;
  return GetClass()
      ->GetDefaultObject<AVNPlayerPaperCharacter>()
      ->InventoryAmmo[AmmoType];
}
void AVNPlayerPaperCharacter::OnEquipPistol() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (Inventory[i]->GetWeaponType() == EWeaponType::Pistol) {
        EquipWeapon(Inventory[i]);
        return;
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipShotgun() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (Inventory[i]->GetWeaponType() == EWeaponType::Shotgun) {
        EquipWeapon(Inventory[i]);
        return;
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipRifle() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (Inventory[i]->GetWeaponType() == EWeaponType::Rifle) {
        EquipWeapon(Inventory[i]);
        return;
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipDoubleBarrel() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (Inventory[i]->GetWeaponType() == EWeaponType::DoubleBarrel) {
        EquipWeapon(Inventory[i]);
        return;
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipGrenadeLauncher() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (Inventory[i]->GetWeaponType() == EWeaponType::GrenadeLauncher) {
        EquipWeapon(Inventory[i]);
        return;
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipPlasmaRifle() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (Inventory[i]->GetWeaponType() == EWeaponType::PlasmaRifle) {
        EquipWeapon(Inventory[i]);
        return;
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipFirst() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (CurrentWeapon->GetWeaponType() == EWeaponType::Pistol) {
        if (Inventory[i]->GetWeaponType() == EWeaponType::SniperRifle) {
          EquipWeapon(Inventory[i]);
          return;
        }
      } else {
        if (Inventory[i]->GetWeaponType() == EWeaponType::Pistol) {
          EquipWeapon(Inventory[i]);
          return;
        }
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipSecond() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (CurrentWeapon->GetWeaponType() == EWeaponType::Shotgun) {
        if (Inventory[i]->GetWeaponType() == EWeaponType::DoubleBarrel) {
          EquipWeapon(Inventory[i]);
          return;
        }
      } else {
        if (Inventory[i]->GetWeaponType() == EWeaponType::Shotgun) {
          EquipWeapon(Inventory[i]);
          return;
        }
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipThird() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (CurrentWeapon->GetWeaponType() == EWeaponType::Rifle) {
        if (Inventory[i]->GetWeaponType() == EWeaponType::MachineGun) {
          EquipWeapon(Inventory[i]);
          return;
        }
      } else {
        if (Inventory[i]->GetWeaponType() == EWeaponType::Rifle) {
          EquipWeapon(Inventory[i]);
          return;
        }
      }
    }
  }
}

void AVNPlayerPaperCharacter::OnEquipFourth() {
  if (Inventory.Num() > 0) {
    for (int32 i = 0; i < Inventory.Num(); i++) {
      if (CurrentWeapon->GetWeaponType() == EWeaponType::GrenadeLauncher) {
        if (Inventory[i]->GetWeaponType() == EWeaponType::PlasmaRifle) {
          EquipWeapon(Inventory[i]);
          return;
        }
      } else {
        if (Inventory[i]->GetWeaponType() == EWeaponType::GrenadeLauncher) {
          EquipWeapon(Inventory[i]);
          return;
        }
      }
    }
  }
}
