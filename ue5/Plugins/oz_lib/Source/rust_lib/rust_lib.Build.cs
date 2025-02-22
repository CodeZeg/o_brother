using System.IO;
using UnrealBuildTool;

public class rust_lib : ModuleRules
{
    public rust_lib(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
        
        // 添加 FlatBuffers 的包含路径
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "ThirdParty", "FlatBuffers", "include"));

        // 添加 FlatBuffers 的库路径
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "ThirdParty", "FlatBuffers", "lib", "flatbuffers.lib"));
    }
}