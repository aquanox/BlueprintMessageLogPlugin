// Copyright 2022, Aquanox.

using UnrealBuildTool;

public class BlueprintMessageEditor : ModuleRules
{
	public BlueprintMessageEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Disable private/public structure
		PrivateIncludePaths.Add(ModuleDirectory);

		if (Target.Configuration == UnrealTargetConfiguration.DebugGame)
		{
			bTreatAsEngineModule = true;
			bUseUnity = false;
		}

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"Kismet",
			"KismetCompiler",
			"BlueprintMessage",
			"BlueprintGraph",
			"GraphEditor",
			"PropertyPath",
			"ToolMenus"
		});
	}
}