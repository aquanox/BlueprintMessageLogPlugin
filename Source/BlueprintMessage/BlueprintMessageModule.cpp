// Copyright 2022, Aquanox.

#include "BlueprintMessageModule.h"
#include "BlueprintMessage.h"
#include "MessageLogModule.h"

IMPLEMENT_MODULE(FBlueprintMessageModule, BlueprintMessage);

DEFINE_LOG_CATEGORY(LogBlueprintMessage);

void FBlueprintMessageModule::StartupModule()
{
	for (FName Name : GetDefault<UBlueprintMessage>()->CustomCategories)
	{
		RegisterCategory(Name);
	}
}

void FBlueprintMessageModule::RegisterCategory(const FName& InName, FText InLabel)
{
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	if (InLabel.IsEmpty())
	{
		InLabel = FText::FromString(FName::NameToDisplayString(InName.ToString(), false));
	}
	MessageLogModule.RegisterLogListing(InName, InLabel);
}

void FBlueprintMessageModule::ShutdownModule()
{
}

