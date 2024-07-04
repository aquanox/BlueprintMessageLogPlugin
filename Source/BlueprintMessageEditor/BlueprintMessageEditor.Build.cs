// Copyright 2022, Aquanox.

using UnrealBuildTool;

public class BlueprintMessageEditor : ModuleRules
{
	private bool bStrictIncludesCheck = false;

	public BlueprintMessageEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Disable private/public structure
		PublicIncludePaths.Add(ModuleDirectory);

		// This is to emulate engine installation and verify includes during development
		// Gives effect similar to BuildPlugin with -StrictIncludes
		if (bStrictIncludesCheck)
		{
			bUseUnity = false;
			PCHUsage = PCHUsageMode.NoPCHs;
			// Enable additional checks used for Engine modules
			bTreatAsEngineModule = true;
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