// Copyright 2022, Aquanox.
#pragma once

#include "BlueprintGraph/GraphPanelPinFactoryBase.h"
#include "BlueprintMessageGraphPanelPinFactory.generated.h"

UCLASS(MinimalAPI, Config=Engine, DefaultConfig)
class UBlueprintMessageGraphPanelPinFactory  : public UGraphPanelPinFactoryBase
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<FGraphPanelPinFactory> CreateFactory() const override;
private:
	TSharedPtr<SGraphPin> CreateGetOptionsPin(UEdGraphPin* Pin) const;
	bool BuildSelectableOptions(TArray<UObject*>& SourceObjects, FString SourceFunctionName, TArray<TSharedPtr<FName>>& OutOptions) const;
};
