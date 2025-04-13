// Copyright 2022, Aquanox.

using UnrealBuildTool;

public class BlueprintMessage : ModuleRules
{
	private bool bStrictIncludesCheck = false;
	private bool bUseCategoryDiscovery = true;

	public BlueprintMessage(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Disable private/public structure
		PublicIncludePaths.Add(ModuleDirectory);

		if (Target.bBuildDeveloperTools && Target.bBuildEditor)
		{
			PublicDependencyModuleNames.Add("MessageLog");

			// Requires for category discovery from viewmodel
			if (bUseCategoryDiscovery)
			{
				var EngineDir = System.IO.Path.GetFullPath(Target.RelativeEnginePath);
				PrivateIncludePaths.Add(EngineDir + "/Source/Developer/MessageLog/Private");
				PrivateDefinitions.Add("WITH_MESSAGELOG_DISCOVERY=1");
			}
		}

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
			"DeveloperSettings"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"UMGEditor",
				"Blutility"
			});
		}
	}
}
