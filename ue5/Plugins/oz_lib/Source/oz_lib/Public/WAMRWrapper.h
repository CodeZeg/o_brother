// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputDataWrapper.h"
#include "RenderDataWrapper.h"
#include "render_data_generated.h"
#include "wasm_export.h"

/**
 * 
 */
class WAMRWrapper
{
public:
	static void LoadDLL();
	static bool wasm_runtime_init();
	static bool wasm_runtime_full_init(RuntimeInitArgs *init_args);
private:
	inline static void* gp_dll_handle = nullptr;
	inline static bool (*vm_init)() = nullptr;
	inline static bool (*vm_full_init)(RuntimeInitArgs *init_args) = nullptr;
};