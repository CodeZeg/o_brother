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
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/warm/libiwasm.dll"));
#endif // PLATFORM_WINDOWS

	gp_dll_handle = FPlatformProcess::GetDllHandle(*LibraryPath);
	vm_init = static_cast<void(*)()>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("wasm_runtime_init")));
}

void WAMRWrapper::wasm_runtime_init()
{
	vm_init();
}