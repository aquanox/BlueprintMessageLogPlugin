#include "BlueprintMessageHelpers.h"
#include "BlueprintMessageToken.h"

#if WITH_EDITOR
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#endif

void FBlueprintMessageHelpers::InvokeDelegate(FBlueprintMessageActionDelegate InDelegate)
{
	if (IsValid(InDelegate.GetUObject()))
	{
		InDelegate.ExecuteIfBound();
	}
}

void FBlueprintMessageHelpers::SpawnEditorUtilityWidget(TSoftObjectPtr<UBlueprint> WidgetBP)
{
#if WITH_EDITOR
	if (UObject* WidgetClass = WidgetBP.LoadSynchronous())
	{
		if (UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(WidgetClass))
		{
			GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>()->SpawnAndRegisterTab(EditorWidget);
		}
	}
#else
	ensureAlwaysMsgf(false, TEXT("Unable to execute action in non-editor build"));
#endif
}
