// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputDataWrapper.h"
#include "RenderDataWrapper.h"
#include "render_data_generated.h"

/**
 * 
 */
class WAMRWrapper
{
public:
	static void LoadDLL();
	static void wasm_runtime_init();
private:
	inline static void* gp_dll_handle = nullptr;
	inline static void (*vm_init)() = nullptr;
};