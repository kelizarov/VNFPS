// Fill out your copyright notice in the Description page of Project Settings.

#include "VNPlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VNPlayerPaperCharacter.h"

AVNPlayerCameraManager::AVNPlayerCameraManager(
    const class FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer) {
  MaxCrouchZOffset = 46.f;

  CrouchLerpVelocity = 12.0f;

  DefaultCameraZOffset = 50.f;

  MaxSlideRotation = 45.f;

  SlideLerpVelocity = 12.f;
}

void AVNPlayerCameraManager::BeginPlay() { Super::BeginPlay(); }

void AVNPlayerCameraManager::UpdateCamera(float DeltaTime) {
  AVNPlayerPaperCharacter *MyPawn =
      Cast<AVNPlayerPaperCharacter>(PCOwner->GetPawn());
  if (MyPawn) {
    const float TargetFOV = MyPawn->IsTargeting() ? MyPawn->GetTargetingFOV()
                                                  : MyPawn->GetNormalFOV();
    DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
    SetFOV(DefaultFOV);
  }
  if (MyPawn) {
    if (MyPawn->bIsCrouched && !bWasCrouched &&
        MyPawn->GetCharacterMovement()->MovementMode !=
            EMovementMode::MOVE_Falling) {
      CurrentCrouchOffset = MaxCrouchZOffset;
    } else if (!MyPawn->bIsCrouched && bWasCrouched &&
               MyPawn->GetCharacterMovement()->MovementMode !=
                   EMovementMode::MOVE_Falling) {
      CurrentCrouchOffset -= MaxCrouchZOffset;
    }
    bWasCrouched = MyPawn->bIsCrouched;

    CurrentCrouchOffset =
        FMath::Lerp(CurrentCrouchOffset, 0.f,
                    FMath::Clamp(CrouchLerpVelocity * DeltaTime, 0.f, 1.0f));
    FVector CurrentCameraPosition =
        MyPawn->GetCamera()->GetRelativeTransform().GetLocation();
    FVector NewCameraPosition =
        FVector(CurrentCameraPosition.X, CurrentCameraPosition.Y,
                DefaultCameraZOffset + CurrentCrouchOffset);
    MyPawn->GetCamera()->SetRelativeLocation(NewCameraPosition);
  }
  Super::UpdateCamera(DeltaTime);
}
