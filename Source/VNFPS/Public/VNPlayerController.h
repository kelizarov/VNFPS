// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "VNPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VNFPS_API AVNPlayerController : public APlayerController
{
	GENERATED_BODY()

	AVNPlayerController(const class FObjectInitializer& ObjectInitializer);
	
	virtual void SetupInputComponent() override;


protected:

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		float AccelerationFriction;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		float AccelerationFactor;

public:

	virtual void LookUpRate(float Val);

	virtual void TurnRate(float Val);
};
