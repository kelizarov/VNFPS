// Fill out your copyright notice in the Description page of Project Settings.

#include "VNFPS.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, VNFPS, "VNFPS" );

//General Log
DEFINE_LOG_CATEGORY(VNLog);

//Logging during game startup
DEFINE_LOG_CATEGORY(VNInit);

//Logging for your AI system
DEFINE_LOG_CATEGORY(VNAI);

//Logging for Critical Errors that must always be addressed
DEFINE_LOG_CATEGORY(VNCriticalErrors);