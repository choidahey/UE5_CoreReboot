// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHong1, Log, All);

#pragma region CR4S_Log

#define FUNCTION_TCHAR (ANSI_TO_TCHAR(__FUNCTION__))
#define CR4S_Log(LogCategory, Verbosity, Format, ...) UE_LOG(LogCategory, Verbosity, TEXT("[%s] %s"), FUNCTION_TCHAR, *FString::Printf(Format, ##__VA_ARGS__))

#pragma endregion