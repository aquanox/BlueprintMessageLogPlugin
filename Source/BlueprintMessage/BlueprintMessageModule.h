#pragma once

#include "Modules/ModuleManager.h"
#include "BlueprintMessageToken.h"

class BLUEPRINTMESSAGE_API FBlueprintMessageModule : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
