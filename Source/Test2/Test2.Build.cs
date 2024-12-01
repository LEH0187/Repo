// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Test2 : ModuleRules
{
	public Test2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] {
			"Test2"
		});

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"ProceduralMeshComponent",
			"FastNoise",
			"FastNoiseGenerator",
			"RenderCore",  // 렌더링 관련 핵심 모듈
			"RHI",         // Render Hardware Interface
			"Renderer"
		});
		PrivateDependencyModuleNames.AddRange(new string[] { });

		if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });  // UnrealEd 모듈 추가
        }
	}
}
