// Copyright 2022, Aquanox.

#pragma once

#include "EdGraphUtilities.h"
#include "GraphPanelPinFactoryBase.generated.h"

/**
 * Self-registering graph panel pin factory
 */
UCLASS(Abstract, NotBlueprintable, NotBlueprintType)
class UGraphPanelPinFactoryBase : public UObject
{
	GENERATED_BODY()
public:
	UGraphPanelPinFactoryBase();

	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	/**
	 * Create new graph panel pin factory instance
	 * @return
	 */
	virtual TSharedPtr<FGraphPanelPinFactory> CreateFactory() const { return nullptr; }

private:

	TWeakPtr<FGraphPanelPinFactory> WeakFactory;
};
