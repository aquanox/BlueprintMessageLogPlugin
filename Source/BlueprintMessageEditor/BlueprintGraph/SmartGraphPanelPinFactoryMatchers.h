// Copyright 2022, Aquanox.

#pragma once

#include "SmartGraphPanelPinFactory.h"
#include "EdGraph/EdGraphSchema.h"
#include "K2Node.h"

struct FCustomPinFactoryMatcher : public FGraphPanelPinMatcher
{
	FCustomPinFactoryMatcher(const FGraphPinMatcherDelegate& InMatcher) : CustomMatcher(InMatcher) { }
	virtual bool Matches(UEdGraphPin& InPin) const override;
private:
	FGraphPinMatcherDelegate CustomMatcher;
};

struct FPinFactoryMatcher_Schema : public FGraphPanelPinMatcher
{
	FPinFactoryMatcher_Schema(const TSubclassOf<UEdGraphSchema>& SchemaClass) : SchemaClass(SchemaClass) {}
	virtual bool Matches(UEdGraphPin& InPin) const override;
private:
	TSubclassOf<UEdGraphSchema> SchemaClass;
};

struct FPinFactoryMatcher_PinCategory : public FGraphPanelPinMatcher
{
	FPinFactoryMatcher_PinCategory(const FName& InName) { Categories.Add(InName); }
	FPinFactoryMatcher_PinCategory(std::initializer_list<FName> InNames) : Categories(InNames) {}
	FPinFactoryMatcher_PinCategory(const TArrayView<FName>& InNames) : Categories(InNames) {}
	virtual bool Matches(UEdGraphPin& InPin) const override;
private:
	TArray<FName> Categories;
};

struct FPinFactoryMatcher_PinSubCategory : public FGraphPanelPinMatcher
{
	FPinFactoryMatcher_PinSubCategory(const FName& InName) { Categories.Add(InName); }
	FPinFactoryMatcher_PinSubCategory(std::initializer_list<FName> InNames) : Categories(InNames) {}
	FPinFactoryMatcher_PinSubCategory(const TArrayView<FName>& InNames) : Categories(InNames) {}
	virtual bool Matches(UEdGraphPin& InPin) const override;
private:
	TArray<FName> Categories;
};

struct FPinFactoryMatcher_PinHasMetadata : public FGraphPanelPinMatcher
{
	FPinFactoryMatcher_PinHasMetadata(const FName& InKey) : MetadataKey(InKey) {}
	virtual bool Matches(UEdGraphPin& InPin) const override;
private:
	FName MetadataKey;
};

struct FPinFactoryMatcher_Node : public FGraphPanelPinMatcher
{
	FPinFactoryMatcher_Node(TSubclassOf<UK2Node> const& InType, bool bExact = false) : bExact(bExact) { NodeTypes.Add(InType); }
	FPinFactoryMatcher_Node(std::initializer_list<TSubclassOf<UK2Node>> InTypes, bool bExact = false) : NodeTypes(InTypes), bExact(bExact) {}
	FPinFactoryMatcher_Node(TArrayView<TSubclassOf<UK2Node>> const& InTypes, bool bExact = false) : NodeTypes(InTypes), bExact(bExact) {}
	virtual bool Matches(UEdGraphPin& InPin) const override;
private:
	TArray<TSubclassOf<UK2Node>> NodeTypes;
	bool bExact = false;
};
