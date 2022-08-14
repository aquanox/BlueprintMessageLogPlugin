#include "BlueprintMessageHelpers.h"
#include "BlueprintMessageToken.h"
#include "BlueprintMessage.h"

#if WITH_EDITOR
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#endif

void FBlueprintMessageHelpers::InvokeDynamicDelegate(FBlueprintMessageActionDelegate InDelegate)
{
	UE_LOG(LogBlueprintMessage, Verbose, TEXT("Invoke external %s"), *InDelegate.ToString<UObject>());

	InDelegate.ExecuteIfBound();
}

void FBlueprintMessageHelpers::SpawnEditorUtilityWidget(TSoftObjectPtr<UBlueprint> WidgetBP)
{
	UE_LOG(LogBlueprintMessage, Verbose, TEXT("Invoke editor utility %s"), *WidgetBP.ToString());

#if WITH_EDITOR
	if (UBlueprint* WidgetClass = WidgetBP.LoadSynchronous())
	{
		if (UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(WidgetClass))
		{
			GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>()->SpawnAndRegisterTab(EditorWidget);
		}
	}
#endif
}
