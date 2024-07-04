// Copyright 2022, Aquanox.

#include "SmartGraphPanelPinFactory.h"

bool FGraphPinMatcherCollection::Matches(UEdGraphPin& Pin) const
{
	for (const TSharedRef<FGraphPanelPinMatcher>& PinMatcher : Matchers)
	{
		if (!PinMatcher->Matches(Pin))
		{
			return false;
		}
	}
	return true;
}

TSharedPtr<SGraphPin> FGraphPinMatcherCollection::Handle(UEdGraphPin* Pin) const
{
	return Handler.Execute(Pin);
}

FSmartGraphPanelPinFactory::FSmartGraphPanelPinFactory()
{
}

TSharedPtr<SGraphPin> FSmartGraphPanelPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (!Pin)
	{
		return nullptr;
	}

	for (const TSharedRef<FGraphPinMatcherCollection>& Handler : Handlers)
	{
		if (Handler->Matches(*Pin))
		{
			TSharedPtr<SGraphPin> CreatedPin = Handler->Handle(Pin);
			if (CreatedPin.IsValid())
			{ // return only if valid, otherwise let other rule to process
				return CreatedPin;
			}
		}
	}
	return nullptr;
}

FGraphPinMatcherCollection& FSmartGraphPanelPinFactory::CreateHandler(const FName& InName)
{
	TSharedRef<FGraphPinMatcherCollection> NewHandler = MakeShared<FGraphPinMatcherCollection>(InName);
	return *Handlers.Add_GetRef(NewHandler);
}
