// Copyright 2022, Aquanox.

#include "Modules/ModuleManager.h"
#include "Misc/EngineVersionComparison.h"
#include "EdGraphUtilities.h"
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
}

void FBlueprintMessageEditorModule::ShutdownModule()
{
#if 1 || UE_VERSION_OLDER_THAN(5, 5, 0)
	FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
	PinFactory.Reset();
#endif
}

