// Copyright 2022, Aquanox.
#pragma once

class FBlueprintMessageHelpers
{
public:

	static void InvokeDynamicDelegate(class FBlueprintMessageActionDelegate InDelegate);

	static void SpawnEditorUtilityWidget(TSoftObjectPtr<UBlueprint> WidgetBP);

	static void GetAvailableCategories(TArray<FName>& OutCategories);
};
