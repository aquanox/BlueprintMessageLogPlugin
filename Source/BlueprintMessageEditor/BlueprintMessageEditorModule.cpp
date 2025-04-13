// Copyright 2022, Aquanox.

#include "BlueprintMessageSettings.h"
#include "Modules/ModuleManager.h"
#include "Misc/EngineVersionComparison.h"
#include "Misc/CoreDelegates.h"
#include "EdGraphUtilities.h"
#include "MessageLogInitializationOptions.h"
#include "MessageLogModule.h"
#include "BlueprintGraph/BlueprintMessageLogPinFactory.h"

struct FBlueprintMessageEditorModule : public FDefaultModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FBlueprintMessageLogPinFactory> PinFactory;
};

IMPLEMENT_MODULE(FBlueprintMessageEditorModule, BlueprintMessageEditor);

void FBlueprintMessageEditorModule::StartupModule()
{
#if 1 || UE_VERSION_OLDER_THAN(5, 5, 0)
	PinFactory = MakeShared<FBlueprintMessageLogPinFactory>();
	PinFactory->Populate();
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
#endif

	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");

	if (GetDefault<UBlueprintMessageSettings>()->bEnableMessageLogDisplay)
	{
		MessageLogModule.EnableMessageLogDisplay(true);
	}

	for (FBlueprintMessageLogCategory Category : GetDefault<UBlueprintMessageSettings>()->CustomCategories)
	{
		if (Category.Name.IsNone())
		{
			continue;
		}
		
		if (Category.DisplayName.IsEmpty())
		{
			Category.DisplayName = FText::FromString(FName::NameToDisplayString(Category.Name.ToString(), false));
		}

		if (Category.bUseAdvancedSettings)
		{
			FMessageLogInitializationOptions InitOptions;
			InitOptions.bShowFilters = Category.bShowFilters;
			InitOptions.bShowPages = Category.bShowPages;
			InitOptions.bAllowClear = Category.bAllowClear;
			InitOptions.bDiscardDuplicates = Category.bDiscardDuplicates;
			InitOptions.MaxPageCount = Category.MaxPageCount;
			InitOptions.bShowInLogWindow = Category.bShowInLogWindow;
			InitOptions.bScrollToBottom = Category.bScrollToBottom;
			MessageLogModule.RegisterLogListing(Category.Name, Category.DisplayName, InitOptions);
		}
		else
		{
			MessageLogModule.RegisterLogListing(Category.Name, Category.DisplayName);
		}
	}

	/*ISettingsModule& Settings = FModuleManager::GetModuleChecked<ISettingsModule>("Settings");
	Settings.RegisterSettings("Editor", "Plugins", "BlueprintMessageLog",
		INVTEXT("Blueprint Message Log"),
		INVTEXT("MessageLog blueprint integration plugin settings"),
		GetMutableDefault<UBlueprintMessageLogSettings>()
	);*/
}


void FBlueprintMessageEditorModule::ShutdownModule()
{
#if 1 || UE_VERSION_OLDER_THAN(5, 5, 0)
	FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
	PinFactory.Reset();
#endif

	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
}
