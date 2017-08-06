// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "VNPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class VNFPS_API AVNPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AVNPlayerCameraManager(const class FObjectInitializer& ObjectInitialzer);

	virtual void UpdateCamera(float DeltaTime) override;
	
	virtual void BeginPlay() override;

private:
	bool bWasCrouched;

	float MaxCrouchZOffset;

	float CrouchLerpVelocity;

	float CurrentCrouchOffset;

	float DefaultCameraZOffset;

	bool bWasSliding;

	float MaxSlideRotation;

	float CurrentCameraSlideRotation;

	float SlideLerpVelocity;


};
