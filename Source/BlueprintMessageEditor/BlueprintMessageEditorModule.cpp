// Copyright 2022, Aquanox.

#include "Modules/ModuleManager.h"
#include "BlueprintMessageCustomPinFactory.h"

struct FBlueprintMessageEditorModule : public FDefaultModuleImpl
{
	virtual void StartupModule() override;

	TSharedPtr<FBlueprintMessageCustomPinFactory> CustomGraphPinFactory;
};

void FBlueprintMessageEditorModule::StartupModule()
{
	CustomGraphPinFactory = MakeShared<FBlueprintMessageCustomPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(CustomGraphPinFactory);
}

IMPLEMENT_MODULE(FBlueprintMessageEditorModule, BlueprintMessageEditor);
