// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE4_SoloPro : ModuleRules
{
	public UE4_SoloPro(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" , "UMG" , "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Slate : 언리얼엔진 HUD시스템을 사용하기위해 필요한 것
	}
}
