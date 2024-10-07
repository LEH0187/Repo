// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Test2 : ModuleRules
{
	public Test2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"ProceduralMeshComponent",
			"FastNoise",
			"FastNoiseGenerator"
		});
		PrivateDependencyModuleNames.AddRange(new string[] { });

		if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });  // UnrealEd 모듈 추가
        }
	}
}
