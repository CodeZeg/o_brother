// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class oz_lib : ModuleRules
{
	public oz_lib(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnableExceptions = true;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects",
				"Engine",
				"CoreUObject",
				"EnhancedInput",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"rust_lib"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		// 添加 gameplay.dll 依赖
		RuntimeDependencies.Add("$(PluginDir)/ThirdParty/Win64/gameplay.dll");

		// 添加wamr库依赖，下面三个步骤是必须的，并且还要将libiwasm.dll放到Plugins/oz_lib/Binaries里
        PublicIncludePaths.Add("$(PluginDir)/ThirdParty/wamr/Win64/Include");
        PublicSystemLibraryPaths.Add("$(PluginDir)/ThirdParty/wamr/Win64/lib");
        PublicAdditionalLibraries.Add("$(PluginDir)/ThirdParty/wamr/Win64/lib/libiwasm.lib");
        // 自动拷贝到Binaries目录
        RuntimeDependencies.Add("$(PluginDir)/Binaries/Win64/libiwasm.dll", "$(PluginDir)/ThirdParty/wamr/Win64/lib/libiwasm.dll");

        // 添加wasm文件
        RuntimeDependencies.Add("$(PluginDir)/ThirdParty/Win64/testapp.wasm");
        RuntimeDependencies.Add("$(PluginDir)/ThirdParty/Win64/testapp.aot");
	}
}
