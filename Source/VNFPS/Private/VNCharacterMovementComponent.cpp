// Fill out your copyright notice in the Description page of Project Settings.

#include "VNCharacterMovementComponent.h"
#include "VNBasePaperCharacter.h"

float UVNCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const AVNBasePaperCharacter* MyPawn = Cast<AVNBasePaperCharacter>(PawnOwner);
	if (MyPawn)
	{
		if (MyPawn->IsTargeting())
		{
			MaxSpeed *= MyPawn->GetTargetingSpeedModifier();
		}
		else if (MyPawn->IsSprinting())
		{
			MaxSpeed *= MyPawn->GetSprintSpeedModifier();
		}
	}
	return MaxSpeed;
}



