#pragma once

#include "Modules/ModuleManager.h"

class BLUEPRINTMESSAGE_API FBlueprintMessageModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterCategory(const FName& InName, FText InLabel = FText());
};
