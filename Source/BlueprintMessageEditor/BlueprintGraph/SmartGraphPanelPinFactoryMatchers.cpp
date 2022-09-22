// Copyright 2022, Aquanox.

#include "BlueprintGraph/SmartGraphPanelPinFactoryMatchers.h"

bool FCustomPinFactoryMatcher::Matches(UEdGraphPin& InPin) const
{
	return CustomMatcher.Execute(InPin);
}

bool FPinFactoryMatcher_Schema::Matches(UEdGraphPin& InPin) const
{
	const UEdGraphSchema* EdGraphSchema = InPin.GetSchema();
	return EdGraphSchema && EdGraphSchema->IsA(SchemaClass);
}

bool FPinFactoryMatcher_PinCategory::Matches(UEdGraphPin& InPin) const
{
	return Categories.Contains(InPin.PinType.PinCategory);
}

bool FPinFactoryMatcher_PinSubCategory::Matches(UEdGraphPin& InPin) const
{
	return Categories.Contains(InPin.PinType.PinSubCategory);
}

bool FPinFactoryMatcher_Node::Matches(UEdGraphPin& InPin) const
{
	if (UObject* PinOuter = InPin.GetOuter())
	{
		UClass* NodeClass = PinOuter->GetClass();
		for (UClass* NodeType : NodeTypes)
		{
			if (NodeClass == NodeType || (!bExact && NodeClass->IsChildOf(NodeType)))
				return true;
		}
	}
	return false;
}

bool FPinFactoryMatcher_PinHasMetadata::Matches(UEdGraphPin& InPin) const
{
	if (UK2Node* PinOuter = Cast<UK2Node>(InPin.GetOuter()))
	{
		FString PinMetaData = PinOuter->GetPinMetaData(InPin.GetFName(), MetadataKey);
		if (!PinMetaData.IsEmpty())
		{
			return true;
		}
	}
	return false;
}
