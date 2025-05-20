// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CR4S : ModuleRules
{
	public CR4S(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
    
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"Slate",
			"SlateCore",
			"ChaosSolverEngine",
			"GeometryCollectionEngine"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "ALS", "ALSCamera" });

		PublicIncludePaths.AddRange(new string[]
		{
			"CR4S",
		});
	}
}