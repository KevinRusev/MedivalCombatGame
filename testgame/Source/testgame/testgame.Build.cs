// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class testgame : ModuleRules
{
	public testgame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"testgame",
			"testgame/Inventory",
			"testgame/Variant_Platforming",
			"testgame/Variant_Platforming/Animation",
			"testgame/Variant_Combat",
			"testgame/Variant_Combat/AI",
			"testgame/Variant_Combat/Animation",
			"testgame/Variant_Combat/Gameplay",
			"testgame/Variant_Combat/Interfaces",
			"testgame/Variant_Combat/UI",
			"testgame/Variant_SideScrolling",
			"testgame/Variant_SideScrolling/AI",
			"testgame/Variant_SideScrolling/Gameplay",
			"testgame/Variant_SideScrolling/Interfaces",
			"testgame/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
