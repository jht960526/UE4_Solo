// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE4_SoloPro : ModuleRules
{
	public UE4_SoloPro(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" , "UMG" , "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Slate : �𸮾��� HUD�ý����� ����ϱ����� �ʿ��� ��
	}
}
