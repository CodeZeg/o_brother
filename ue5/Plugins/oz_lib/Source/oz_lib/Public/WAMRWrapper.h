// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogGP, Log, All);


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