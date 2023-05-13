// Fill out your copyright notice in the Description page of Project Settings.

#include "VNPlayerController.h"
#include "VNPlayerCameraManager.h"

AVNPlayerController::AVNPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AVNPlayerCameraManager::StaticClass();

	AccelerationFriction = 2.f;
	AccelerationFactor = 2.f;
	BaseTurnRate = 75.f;
	BaseLookUpRate = 75.f;
}

void AVNPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	InputComponent->BindAxis("LookUpRate", this, &AVNPlayerController::LookUpRate);
	InputComponent->BindAxis("TurnRate", this, &AVNPlayerController::TurnRate);
	InputComponent->BindAxis("Turn", this, &APlayerController::AddYawInput);
	InputComponent->BindAxis("LookUp", this, &APlayerController::AddPitchInput);

}

void AVNPlayerController::LookUpRate(float Val)
{
	static float CameraSpeed = BaseLookUpRate;

	if (Val != 0.f)
	{
		if (FMath::Abs(Val) > 0.8f)
		{
			if (CameraSpeed < BaseLookUpRate * AccelerationFactor)
				CameraSpeed += AccelerationFriction * FMath::Abs(Val) * GetWorld()->GetDeltaSeconds();
			else
				CameraSpeed = BaseLookUpRate * AccelerationFactor;
		}
		else
		{
			if (CameraSpeed > BaseLookUpRate)
				CameraSpeed -= AccelerationFriction * GetWorld()->GetDeltaSeconds();
			else
				CameraSpeed = BaseLookUpRate;
		}
		GEngine->AddOnScreenDebugMessage(0, 0.001f, FColor::Blue, FString::Printf(TEXT("Val : %f"), CameraSpeed));

		AddPitchInput(Val * CameraSpeed * GetWorld()->GetDeltaSeconds());
	}
	else
		CameraSpeed = BaseLookUpRate;

}

void AVNPlayerController::TurnRate(float Val)
{
	static float CameraSpeed = BaseTurnRate;

	if (Val != 0.f)
	{
		if (FMath::Abs(Val) > 0.8f)
		{
			if (CameraSpeed < BaseTurnRate * AccelerationFactor)
				CameraSpeed += AccelerationFriction * FMath::Abs(Val) * GetWorld()->GetDeltaSeconds();
			else
				CameraSpeed = BaseTurnRate * AccelerationFactor;
		}
		else
		{
			if (CameraSpeed > BaseTurnRate)
				CameraSpeed -= AccelerationFriction * GetWorld()->GetDeltaSeconds();
			else
				CameraSpeed = BaseTurnRate;
		}
		GEngine->AddOnScreenDebugMessage(1, 0.001f, FColor::Red, FString::Printf(TEXT("Val : %f"), CameraSpeed));

		AddYawInput(Val * CameraSpeed * GetWorld()->GetDeltaSeconds());
	}
	else
		CameraSpeed = BaseTurnRate;
}