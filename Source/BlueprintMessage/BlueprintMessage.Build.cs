// Copyright 2022, Aquanox.

using UnrealBuildTool;

public class BlueprintMessage : ModuleRules
{
	public BlueprintMessage(ReadOnlyTargetRules Target) : base(Target)
	{

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Disable private/public structure
		PublicIncludePaths.Add(ModuleDirectory);
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
			"Engine"
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