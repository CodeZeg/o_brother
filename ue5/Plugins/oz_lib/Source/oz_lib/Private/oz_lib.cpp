// Copyright Epic Games, Inc. All Rights Reserved.

#include "oz_lib.h"

#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "Foz_libModule"

// extern "C" { __declspec(dllimport) int q_add(int a, int b); }

// typedef int(*StartFunc)(int a, int b);
void Foz_libModule::StartupModule()
{
// 	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
//
// 	// Get the base directory of this plugin
// 	FString BaseDir = IPluginManager::Get().FindPlugin("oz_lib")->GetBaseDir();
//
// 	// Add on the relative location of the third party dll and load it
// 	FString LibraryPath;
// #if PLATFORM_WINDOWS
// 	LibraryPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/Win64/gameplay.dll"));
// #elif PLATFORM_MAC
//     LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/Mac/Release/gameplay.dylib"));
// #elif PLATFORM_LINUX
// 	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/Linux/x86_64-unknown-linux-gnu/gameplay.so"));
// #endif // PLATFORM_WINDOWS
//
// 	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
//
// 	if (ExampleLibraryHandle)
// 	{
// 		// Call the test function in the third party library that opens a message box
// 		UE_LOG(LogTemp, Display, TEXT("Load Lib Success : %s"), *LibraryPath);
// 		StartFunc q_add_d = (StartFunc)FPlatformProcess::GetDllExport(ExampleLibraryHandle, TEXT("q_add"));
// 		int aaa = q_add_d(11, 55);
// 		UE_LOG(LogTemp, Display, TEXT("Load Lib q_add : %d"), aaa);
// 		// int bbb = q_add(123, 456);
// 		// UE_LOG(LogTemp, Display, TEXT("Load Lib q_add : %d"), bbb);
// 		UE_LOG(LogTemp, Display, TEXT("after Load Lib Success : %s"), *LibraryPath);
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Load Lib Failed : %s"), *LibraryPath);
// 		// FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError xxxx", "xxxxx Failed to load example third party library"));
// 	}
}

void Foz_libModule::ShutdownModule()
{
	// // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// // we call this function before unloading the module.
	//
	// // Free the dll handle
	// FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	// ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(Foz_libModule, oz_lib)
