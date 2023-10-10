// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GachonGameProject2 : ModuleRules
{
	public GachonGameProject2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
