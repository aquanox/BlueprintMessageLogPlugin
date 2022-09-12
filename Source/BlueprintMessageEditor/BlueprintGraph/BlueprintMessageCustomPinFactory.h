// Copyright 2022, Aquanox.
#pragma once

#include "RuleBasedGraphPanelPinFactory.h"

struct FBlueprintMessageCustomPinFactory  : public FRuleBasedGraphPanelPinFactory
{
	FBlueprintMessageCustomPinFactory();
protected:
	TSharedPtr<SGraphPin> CreateGetOptionsPin(UEdGraphPin* Pin) const;
	void GetOptionsToSelect(TArray<UObject*>& SourceObjects, FString SourceFunctionName, TArray<TSharedPtr<FName>>& OutOptions) const;

};
