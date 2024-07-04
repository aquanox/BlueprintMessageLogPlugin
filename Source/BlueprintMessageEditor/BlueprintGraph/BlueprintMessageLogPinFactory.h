// Copyright 2022, Aquanox.
#pragma once

#include "SmartGraphPanelPinFactory.h"

class FBlueprintMessageLogPinFactory  : public FSmartGraphPanelPinFactory
{
	using ThisClass = FBlueprintMessageLogPinFactory;
public:
	void Populate();

	TSharedPtr<SGraphPin> CreateGetOptionsPin(UEdGraphPin* Pin) const;
	bool BuildSelectableOptions(TArray<UObject*>& SourceObjects, FString SourceFunctionName, TArray<TSharedPtr<FName>>& OutOptions) const;
};
