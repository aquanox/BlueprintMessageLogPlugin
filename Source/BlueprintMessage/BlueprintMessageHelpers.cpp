// Copyright 2022, Aquanox.
#include "BlueprintMessageHelpers.h"
#include "BlueprintMessageToken.h"
#include "BlueprintMessage.h"
#include "MessageLogModule.h"
#include "AccessPrivate.h"
#include "Algo/RemoveIf.h"

#if WITH_EDITOR
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"

#include "Developer/MessageLog/Private/Presentation/MessageLogViewModel.h"
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

#if WITH_EDITOR

class FMessageLogViewModel;
using ViewModelList = TSharedPtr<FMessageLogViewModel>;
class FMessageLogListingViewModel;
using ViewModelNameMap = TMap< FName, TSharedPtr<FMessageLogListingViewModel> >;

IMPLEMENT_GET_PRIVATE_VAR(FMessageLogModule, MessageLogViewModel, ViewModelList);
IMPLEMENT_GET_PRIVATE_VAR(FMessageLogViewModel, NameToViewModelMap, ViewModelNameMap);

#endif

void FBlueprintMessageHelpers::GetAvailableCategories(TArray<FName>& OutCategories)
{
	static TOptional<TArray<FName>> CachedResult;
	if (!CachedResult.IsSet())
	{
		TArray<FName> Keys;
#if WITH_EDITOR
		FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
		auto& ViewModel = MessageLogModule.*GetPrivate(FMessageLogModuleMessageLogViewModelAccessor());
		auto& ViewModelNames = *ViewModel.*GetPrivate(FMessageLogViewModelNameToViewModelMapAccessor());

		ViewModelNames.GetKeys(Keys);

		for (auto It = Keys.CreateIterator(); It; ++It)
		{
			FString AsString = It->ToString();
			if (AsString.Contains(TEXT("PROTO")))
			{
				It.RemoveCurrent();
			}
		}
#endif
		CachedResult = MoveTemp(Keys);
	}

	OutCategories = CachedResult.GetValue();
}
