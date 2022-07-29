// Copyright 2022, Aquanox.

#include "Modules/ModuleManager.h"

class FBlueprintMessageModule : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FBlueprintMessageModule, BlueprintMessage);

void FBlueprintMessageModule::StartupModule()
{
	FDefaultModuleImpl::StartupModule();
}

void FBlueprintMessageModule::ShutdownModule()
{
	FDefaultModuleImpl::ShutdownModule();
}
