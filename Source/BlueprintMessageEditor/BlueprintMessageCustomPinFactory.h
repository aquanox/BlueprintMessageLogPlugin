// Copyright 2022, Aquanox.
#pragma once

#include "EdGraphUtilities.h"

struct FBlueprintMessageCustomPinFactory  : public FGraphPanelPinFactory
{
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};
