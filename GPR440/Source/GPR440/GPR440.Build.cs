// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GPR440 : ModuleRules
{
	public GPR440(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
