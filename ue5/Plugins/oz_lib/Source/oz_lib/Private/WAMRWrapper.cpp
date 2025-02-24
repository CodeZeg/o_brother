// Fill out your copyright notice in the Description page of Project Settings.


#include "WAMRWrapper.h"
#include "render_data_generated.h"

#include "Interfaces/IPluginManager.h"

void WAMRWrapper::LoadDLL()
{
	if(gp_dll_handle)
		return;

	FString BaseDir = IPluginManager::Get().FindPlugin("oz_lib")->GetBaseDir();
	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/wamr/libiwasm.dll"));
#endif // PLATFORM_WINDOWS

	gp_dll_handle = FPlatformProcess::GetDllHandle(*LibraryPath);
	vm_init = static_cast<bool(*)()>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("wasm_runtime_init")));
	vm_full_init = static_cast<bool(*)(RuntimeInitArgs *)>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("wasm_runtime_full_init")));
}

bool WAMRWrapper::wasm_runtime_init()
{
	vm_init();
	return true;
}

bool WAMRWrapper::wasm_runtime_full_init(RuntimeInitArgs *init_args)
{
	vm_full_init(init_args);
	return true;
}