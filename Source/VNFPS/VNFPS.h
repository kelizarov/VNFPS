// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "VNBasePaperCharacter.h"

#define COLLISION_WEAPON			ECC_GameTraceChannel1
#define COLLISION_PROJECTILE		ECC_GameTraceChannel2
#define COLLISION_PICKUP			ECC_GameTraceChannel3

//General Log
DECLARE_LOG_CATEGORY_EXTERN(VNLog, Log, All);

//Logging during game startup
DECLARE_LOG_CATEGORY_EXTERN(VNInit, Log, All);

//Logging for your AI system
DECLARE_LOG_CATEGORY_EXTERN(VNAI, Log, All);

//Logging for Critical Errors that must always be addressed
DECLARE_LOG_CATEGORY_EXTERN(VNCriticalErrors, Log, All);