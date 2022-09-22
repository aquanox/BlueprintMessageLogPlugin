// Copyright 2022, Aquanox.

#include "BlueprintGraph/GraphPanelPinFactoryBase.h"
#include "EdGraphUtilities.h"

UGraphPanelPinFactoryBase::UGraphPanelPinFactoryBase()
{
}

void UGraphPanelPinFactoryBase::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject)
		&& GetClass() != StaticClass()
		&& !GetClass()->HasAnyClassFlags(CLASS_Abstract))
	{
		TSharedPtr<FGraphPanelPinFactory> Factory = CreateFactory();
		FEdGraphUtilities::RegisterVisualPinFactory(Factory);
		WeakFactory = Factory;
	}
}

void UGraphPanelPinFactoryBase::BeginDestroy()
{
	Super::BeginDestroy();

	TSharedPtr<FGraphPanelPinFactory> Factory = WeakFactory.Pin();
	if (Factory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(Factory);
		WeakFactory.Reset();
	}
}
