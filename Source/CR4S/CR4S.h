// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHong1, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSetting, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGimmick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEnvironment, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDa, Log, All);

#define FUNCTION_TCHAR (ANSI_TO_TCHAR(__FUNCTION__))
#pragma region CR4S_Log

#define CR4S_Log_Explicit(LogCategory, Verbosity, InFunctionName, InLineNumber, Format, ...) \
    UE_LOG(LogCategory, Verbosity, TEXT("[%s::%d] %s"), InFunctionName, InLineNumber, *FString::Printf(Format, ##__VA_ARGS__))

#define CR4S_Log(LogCategory, Verbosity, Format, ...) \
    CR4S_Log_Explicit(LogCategory, Verbosity, FUNCTION_TCHAR, __LINE__, Format, ##__VA_ARGS__)

#pragma endregion

#pragma region CR4S_Validate

#define CR4S_VALIDATE(LogCategory, Expression) \
    [&, OuterFunctionName = FUNCTION_TCHAR, OuterLineNumber = __LINE__]() -> bool { \
        if (!(Expression)) { \
            CR4S_Log_Explicit(LogCategory, Warning, OuterFunctionName, OuterLineNumber, TEXT("Validation Failed: %s"), TEXT(#Expression)); \
            return false; \
        } \
        return true; \
    }()

#pragma endregion