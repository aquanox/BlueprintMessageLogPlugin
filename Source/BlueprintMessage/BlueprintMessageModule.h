#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Internationalization/Text.h"

class BLUEPRINTMESSAGE_API FBlueprintMessageModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterCategory(const FName& InName, FText InLabel = FText());
};
