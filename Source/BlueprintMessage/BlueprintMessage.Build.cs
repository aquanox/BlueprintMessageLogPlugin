// Copyright 2022, Aquanox.

using UnrealBuildTool;

public class BlueprintMessage : ModuleRules
{
	private bool bStrictIncludesCheck = false;

	public BlueprintMessage(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Disable private/public structure
		PublicIncludePaths.Add(ModuleDirectory);

		if (Target.bBuildEditor)
		{
			var EngineDir = System.IO.Path.GetFullPath(Target.RelativeEnginePath);
			PrivateIncludePaths.Add(EngineDir + "/Source/Developer/MessageLog/Private");
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
			"MessageLog"
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