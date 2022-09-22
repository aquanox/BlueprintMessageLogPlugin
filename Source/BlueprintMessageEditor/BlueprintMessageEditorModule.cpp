// Copyright 2022, Aquanox.

#include "Modules/ModuleManager.h"
#include "BlueprintGraph/BlueprintMessageGraphPanelPinFactory.h"

struct FBlueprintMessageEditorModule : public FDefaultModuleImpl
{
	virtual void StartupModule() override;
};

void FBlueprintMessageEditorModule::StartupModule()
{
	// CustomGraphPinFactory = MakeShared<FBlueprintMessageCustomPinFactory>();
	// FEdGraphUtilities::RegisterVisualPinFactory(MakeShared<FBlueprintMessageEditorModule>());
}

IMPLEMENT_MODULE(FBlueprintMessageEditorModule, BlueprintMessageEditor);
