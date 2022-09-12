// Copyright 2022, Aquanox.

#pragma once

#include "SGraphPin.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphUtilities.h"

struct FRuleBasedGraphPanelPinFactory;

struct FPinFactoryMatcher
{
	virtual ~FPinFactoryMatcher() = default;
	virtual bool Matches(UEdGraphPin* InPin) const = 0;
};

struct FPinFactoryMatcher_Schema : FPinFactoryMatcher
{
	FPinFactoryMatcher_Schema(const TSubclassOf<UEdGraphSchema>& SchemaClass);
	virtual bool Matches(UEdGraphPin* InPin) const override;
private:
	TSubclassOf<UEdGraphSchema> SchemaClass;
};

struct FPinFactoryMatcher_PinCategory : FPinFactoryMatcher
{
	FPinFactoryMatcher_PinCategory(const FName& InName);
	FPinFactoryMatcher_PinCategory(std::initializer_list<FName> InNames);
	FPinFactoryMatcher_PinCategory(const TArrayView<FName>& InNames);
	virtual bool Matches(UEdGraphPin* InPin) const override;
private:
	TArray<FName, TInlineAllocator<1>> Categories;
};

struct FPinFactoryMatcher_Node : FPinFactoryMatcher
{
	FPinFactoryMatcher_Node(TSubclassOf<UK2Node> const& InType);
	FPinFactoryMatcher_Node(std::initializer_list<TSubclassOf<UK2Node>> InType);
	FPinFactoryMatcher_Node(TArrayView<TSubclassOf<UK2Node>> const& InType);
	virtual bool Matches(UEdGraphPin* InPin) const override;
private:
	TArray<TSubclassOf<UK2Node>, TInlineAllocator<8>> NodeTypes;
};

struct FPinFactoryMatcher_PinMetadata : FPinFactoryMatcher
{
	FPinFactoryMatcher_PinMetadata(const FName& MetadataKey);
	virtual bool Matches(UEdGraphPin* InPin) const override;
private:
	FName MetadataKey;
};

struct FGraphPanelPinFactoryHandler
{
	friend FRuleBasedGraphPanelPinFactory;

	using FDelegate = TDelegate<TSharedPtr<SGraphPin>(UEdGraphPin*)>;

	/**
	 * @brief
	 * @param Pin
	 * @return
	 */
	bool Matches(UEdGraphPin* Pin) const;

	/**
	 *
	 */
	TSharedPtr<SGraphPin> Handle(UEdGraphPin* Pin) const;

	/**
	 * @brief
	 * @param InMatcher
	 * @return
	 */
	FGraphPanelPinFactoryHandler& AddMatcher(TSharedRef<FPinFactoryMatcher> && InMatcher)
	{
		Matchers.Add(MoveTemp(InMatcher));
		return *this;
	}

	template<typename T, typename... TArgs>
	FGraphPanelPinFactoryHandler& AddMatcher(TArgs&&... Args)
	{
		Matchers.Add(MakeShared<T>(Forward<TArgs>(Args)...));
		return *this;
	}

	/**
	 * @brief
	 * @param InHandler
	 */
	void Handle(FDelegate&& InHandler)
	{
		Handler = MoveTemp(InHandler);
	}
private:
	FName Label;
	TArray<TSharedRef<FPinFactoryMatcher>> Matchers;
	FDelegate Handler;
};

/**
 * @brief Base type for complex pin factories with multiple matches
 */
struct FRuleBasedGraphPanelPinFactory : public FGraphPanelPinFactory
{
	FRuleBasedGraphPanelPinFactory();
protected:
	/**
	 *
	 */
	using FHandlerDelegate = FGraphPanelPinFactoryHandler::FDelegate;
	/**
	 * @brief
	 * @param Pin
	 * @return
	 */
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override final;
	/**
	 * @brief Create new handler with label
	 * @param InName handler label
	 * @return new handler instance
	 */
	FGraphPanelPinFactoryHandler& CreateHandler(FName InName);
private:
	/**
	 * @brief
	 */
	TArray<TSharedRef<FGraphPanelPinFactoryHandler>> Handlers;
};
