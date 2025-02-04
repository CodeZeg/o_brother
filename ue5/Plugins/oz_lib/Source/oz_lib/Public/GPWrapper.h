// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGP, Log, All);



// Equivalent to FightingInputStateData
struct FightingInputStateData {
	float direction; // 移动方向
	float magnitude; // 意向强度
};

// Tag for InputStateData enum
enum InputStateData_Tag {
	InputStateData_Fighting,
	InputStateData_Other
};

// Equivalent to InputStateData
struct InputStateData {
	InputStateData_Tag tag;
	union {
		FightingInputStateData fighting; // Corresponds to Fighting variant
		// No data for Other variant
	} data;
};

// Tag for InputActionData enum
enum InputActionData_Tag {
	None,
	InputActionData_ChoseCard
	// Add other variants here if necessary
};

// Equivalent to InputActionData
struct InputActionData {
	InputActionData_Tag tag;
	union {
		uint8_t choseCard; // Corresponds to ChoseCard variant
		// Add other variant data here if necessary
	} data;
};

// Equivalent to InputPlayerData
struct InputPlayerData {
	InputStateData state;
	InputActionData action;
};

// Equivalent to InputData
struct InputData {
	InputPlayerData player_0;
};

/**
 * 
 */
class OZ_LIB_API GPWrapper
{
public:
	static void LoadDLL();
	static void UnLoadDLL();
	static void Start();
	static void Update(float DeltaTime);
private:
	inline static void* gp_dll_handle = nullptr;
	inline static int (*gp_q_add)(int, int) = nullptr;
	inline static void (*gp_start)() = nullptr;
	inline static void (*gp_update)(InputData) = nullptr;
};