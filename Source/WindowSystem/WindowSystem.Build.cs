// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;
public class WindowSystem : ModuleRules
{
	public WindowSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        if (UnrealTargetPlatform.Win64 == Target.Platform)
        {
			// Nuklear
            string Path_Nuklear = "../Source/WindowSystem/ThirdParty/nuklear";
            PrivateIncludePaths.Add(Path_Nuklear);

			// Imgui (FUTURE)
			string Path_Imgui = "../Source/WindowSystem/ThirdParty/imgui";
            PrivateIncludePaths.Add(Path_Imgui);
        }

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"RenderCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}