// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TopDownShooteUE4 : ModuleRules
{
	public TopDownShooteUE4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "PhysicsCore", "Niagara" });
    }
}
