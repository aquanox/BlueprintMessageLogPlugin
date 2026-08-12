// Copyright 2022, Aquanox.

#include "BlueprintMessageSettings.h"
#include "Modules/ModuleManager.h"
#include "Misc/EngineVersionComparison.h"
#include "EdGraphUtilities.h"
#include "MessageLogInitializationOptions.h"
#include "MessageLogModule.h"
#include "BlueprintGraph/BlueprintMessageLogPinFactory.h"

#define WITH_CUSTOM_GETOPTIONS  UE_VERSION_OLDER_THAN(5, 5, 0)

struct FBlueprintMessageEditorModule : public FDefaultModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override { return false; }

	TSharedPtr<FBlueprintMessageLogPinFactory> PinFactory;
};

IMPLEMENT_MODULE(FBlueprintMessageEditorModule, BlueprintMessageEditor);

void FBlueprintMessageEditorModule::StartupModule()
{
	if (!GIsEditor || IsRunningCommandlet())
	{
		return;
	}

#if WITH_CUSTOM_GETOPTIONS
	PinFactory = MakeShared<FBlueprintMessageLogPinFactory>();
	PinFactory->Populate();
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
#endif

	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");

	if (UBlueprintMessageSettings::Get()->bEnableMessageLogDisplay)
	{
		MessageLogModule.EnableMessageLogDisplay(true);
	}

	for (FBlueprintMessageLogCategory Category : UBlueprintMessageSettings::Get()->CustomCategories)
	{
		if (Category.Name.IsNone() || !Category.bAutoRegisterCategory)
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
}

void FBlueprintMessageEditorModule::ShutdownModule()
{
	if (!GIsEditor || IsRunningCommandlet())
	{
		return;
	}

#if WITH_CUSTOM_GETOPTIONS
	FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
	PinFactory.Reset();
#endif
}
