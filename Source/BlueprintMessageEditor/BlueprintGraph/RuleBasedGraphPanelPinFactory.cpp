// Copyright 2022, Aquanox.

#include "BlueprintGraph/RuleBasedGraphPanelPinFactory.h"

#include "K2Node_CallFunction.h"

FPinFactoryMatcher_Schema::FPinFactoryMatcher_Schema(const TSubclassOf<UEdGraphSchema>& SchemaClass)
	: SchemaClass(SchemaClass)
{
}

bool FPinFactoryMatcher_Schema::Matches(UEdGraphPin* InPin) const
{
	return InPin->GetSchema() && InPin->GetSchema()->IsA(SchemaClass);
}

FPinFactoryMatcher_PinCategory::FPinFactoryMatcher_PinCategory(const FName& InName)
{
	Categories.Add(InName);
}

FPinFactoryMatcher_PinCategory::FPinFactoryMatcher_PinCategory(std::initializer_list<FName> InNames)
{
	Categories.Append(InNames);
}

FPinFactoryMatcher_PinCategory::FPinFactoryMatcher_PinCategory(const TArrayView<FName>& InNames)
{
	Categories.Append(InNames.GetData(), InNames.Num());
}


bool FPinFactoryMatcher_PinCategory::Matches(UEdGraphPin* InPin) const
{
	return InPin && Categories.Contains(InPin->PinType.PinCategory);
}

FPinFactoryMatcher_Node::FPinFactoryMatcher_Node(TSubclassOf<UK2Node> const& InType)
{
	NodeTypes.Add(InType);
}

FPinFactoryMatcher_Node::FPinFactoryMatcher_Node(std::initializer_list<TSubclassOf<UK2Node>> InType)
{
	NodeTypes.Append(InType);
}

FPinFactoryMatcher_Node::FPinFactoryMatcher_Node(TArrayView<TSubclassOf<UK2Node>> const& InType)
{
	NodeTypes.Append(InType.GetData(), InType.Num());
}

bool FPinFactoryMatcher_Node::Matches(UEdGraphPin* InPin) const
{
	if (InPin)
	{
		if (UObject* PinOuter = InPin->GetOuter())
		{
			UClass* NodeClass = PinOuter->GetClass();
			for (UClass* NodeType : NodeTypes)
			{
				if (NodeClass == NodeType || NodeClass->IsChildOf(NodeType))
					return true;
			}
		}
	}
	return false;
}

FPinFactoryMatcher_PinMetadata::FPinFactoryMatcher_PinMetadata(const FName& MetadataKey): MetadataKey(MetadataKey)
{

}

bool FPinFactoryMatcher_PinMetadata::Matches(UEdGraphPin* InPin) const
{
	if (InPin)
	{
		if (UK2Node* PinOuter = Cast<UK2Node>(InPin->GetOuter()))
		{
			FString PinMetaData = PinOuter->GetPinMetaData(InPin->GetFName(), MetadataKey);
			if (!PinMetaData.IsEmpty())
			{
				return true;
			}
		}
	}
	return false;
}


bool FGraphPanelPinFactoryHandler::Matches(UEdGraphPin* Pin) const
{
	for (auto& PinMatcher : Matchers)
	{
		if (!PinMatcher->Matches(Pin))
		{
			return false;
		}
	}
	return true;
}

TSharedPtr<SGraphPin> FGraphPanelPinFactoryHandler::Handle(UEdGraphPin* Pin) const
{
	return Handler.Execute(Pin);
}

FRuleBasedGraphPanelPinFactory::FRuleBasedGraphPanelPinFactory()
{
}

TSharedPtr<SGraphPin> FRuleBasedGraphPanelPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (!Pin)
	{
		return nullptr;
	}

	for (auto& PinHandler : Handlers)
	{
		if (PinHandler->Matches(Pin))
		{
			TSharedPtr<SGraphPin> CreatedPin = PinHandler->Handle(Pin);
			if (CreatedPin.IsValid())
			{ // return only if valid, otherwise let other rule to process
				return CreatedPin;
			}
		}
	}
	return nullptr;
}

FGraphPanelPinFactoryHandler& FRuleBasedGraphPanelPinFactory::CreateHandler(FName InName)
{
	auto NewHandler = MakeShared<FGraphPanelPinFactoryHandler>();
	NewHandler->Label = InName;
	return *Handlers.Add_GetRef(NewHandler);
}
