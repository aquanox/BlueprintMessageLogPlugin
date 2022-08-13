#pragma once

class FBlueprintMessageHelpers
{
public:

	static void InvokeDelegate(class FBlueprintMessageActionDelegate InDelegate);

	static void SpawnEditorUtilityWidget(TSoftObjectPtr<UBlueprint> WidgetBP);
};
