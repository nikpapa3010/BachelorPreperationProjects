// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Highlighter : ModuleRules
{
	public Highlighter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Kismet",
				"KismetCompiler",
				"Blutility",
				"EditorScriptingUtilities",
				"UMG",
				"Niagara",
				"UnrealEd",
				"AssetTools",
				"InputCore",
				"AppFramework",
				"Projects",
				"SceneOutliner",
				"ToolMenus",
				"BlueprintGraph",
				"UMGEditor",
				"InputCore",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Kismet",
				"GraphEditor"

				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}