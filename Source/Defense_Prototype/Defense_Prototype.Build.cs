// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Defense_Prototype : ModuleRules
{
	public Defense_Prototype(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
        PublicIncludePaths.Add(ModuleDirectory);
    }
}
