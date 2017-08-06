// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "VNCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class VNFPS_API UVNCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

		virtual float GetMaxSpeed() const override;
	
	
};
