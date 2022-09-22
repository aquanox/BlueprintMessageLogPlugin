// Copyright 2022, Aquanox.

#pragma once

#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphPin.h"

class SGraphPin;

struct FGraphPanelPinMatcher
{
	FGraphPanelPinMatcher() = default;

	virtual ~FGraphPanelPinMatcher() = default;
	/**
	 * Test if pin satisfies conditions of this matchers
	 * @param InPin pin to test
	 * @return true if matches conditions
	 */
	virtual bool Matches(UEdGraphPin& InPin) const = 0;
};

using FGraphPinMatcherDelegate = TDelegate<bool(UEdGraphPin&)>;
using FGraphPinHandlerDelegate = TDelegate<TSharedPtr<SGraphPin>(UEdGraphPin*)>;

struct FGraphPinMatcherCollection
{
	explicit FGraphPinMatcherCollection(const FString& Label)
		: Label(Label)
	{
	}

	/**
	 * Test if pin can be handled by this
	 * @return true if pin can be handled by this handler
	 */
	bool Matches(UEdGraphPin& Pin) const;

	/**
	 * Invoke handling delegate on pin
	 */
	TSharedPtr<SGraphPin> Handle(UEdGraphPin* Pin) const;

	/**
	 * Add new matcher to collection
	 */
	FGraphPinMatcherCollection& AddMatcher(TSharedRef<FGraphPanelPinMatcher>&& InMatcher)
	{
		Matchers.Add(MoveTemp(InMatcher));
		return *this;
	}

	/**
	 * Add new matcher to collection
	 */
	template <typename T, typename... TArgs>
	FGraphPinMatcherCollection& AddMatcher(TArgs&&... Args)
	{
		Matchers.Add(MakeShared<T>(Forward<TArgs>(Args)...));
		return *this;
	}

	void Handle(const FGraphPinHandlerDelegate& InDelegate)
	{
		Handler = InDelegate;
	}

	void Handle(FGraphPinHandlerDelegate&& InDelegate)
	{
		Handler = MoveTemp(InDelegate);
	}

private:
	FString Label;
	TArray<TSharedRef<FGraphPanelPinMatcher>> Matchers;
	FGraphPinHandlerDelegate Handler;
};

/**
 * Internal graph panel pin factory
 */
class FSmartGraphPanelPinFactory : public FGraphPanelPinFactory
{
public:
	FSmartGraphPanelPinFactory();
	/**
	 * Create new handler with label
	 * @param InLabel handler label
	 * @return new handler instance
	 */
	FGraphPinMatcherCollection& CreateHandler(const FString& InLabel);
protected:
	/**
	 * Create pin widget for specified graph pin, if possible
	 * @param Pin graph pin
	 * @return slate widget for pin, or null if not applicable
	 */
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override final;

	/**
	 *
	 */
	TArray<TSharedRef<FGraphPinMatcherCollection>> Handlers;
};
