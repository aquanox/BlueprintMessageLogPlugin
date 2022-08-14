// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "Logging/TokenizedMessage.h"
#include "BlueprintMessageToken.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(FText, FGetMessageDynamicText);
DECLARE_DYNAMIC_DELEGATE_RetVal(FText, FGetMessageDynamicString);
DECLARE_DYNAMIC_DELEGATE(FBlueprintMessageActionDelegate);

/** Delegate used when clicking a message token */
DECLARE_DELEGATE_OneParam(FOnBlueprintMessageTokenActivated, const struct FBlueprintMessageToken&);

/**
 * Hack around message tokens being non-extensible and hardcoded in engine.
 * Inherit this struct to provide data to token and use constructor taking it
 */
struct BLUEPRINTMESSAGE_API FBlueprintMessageTokenData : public TSharedFromThis<FBlueprintMessageTokenData>
{
public:
};

/**
 * Wrapper-container for generic message token to pass around in blueprint
 */
USTRUCT(BlueprintType, meta=(HiddenByDefault, DisableSplitPin))
struct BLUEPRINTMESSAGE_API FBlueprintMessageToken
{
	GENERATED_BODY()
public:
	FBlueprintMessageToken() = default;
	FBlueprintMessageToken(const FName& InSlot);
	FBlueprintMessageToken(TSharedRef<IMessageToken>&& InToken);
	FBlueprintMessageToken(TSharedRef<IMessageToken>&& InToken, TSharedRef<FBlueprintMessageTokenData>&& InCustomData);

	bool operator==(const FName& Other) const { return Name == Other; }

	/* Get the type of this message token */
	EMessageToken::Type GetType() const { return Instance.IsValid() ? Instance->GetType() : static_cast<EMessageToken::Type>(-1); }
	/* Does this token have custom data present */
	bool HasData() const { return Data.IsValid(); }
	/* Get the custom data for this token of specific type */
	template<typename T = FBlueprintMessageTokenData>
	T& GetData() const { return StaticCastSharedPtr<T, FBlueprintMessageTokenData>(Data); }

	/** Set token activation processor */
	void OnMessageTokenActivated(FOnBlueprintMessageTokenActivated&& Delegate);

	/* Token name */
	FName Name;
	/* Token instance */
	TSharedPtr<IMessageToken> Instance;
	/* Token custom data */
	TSharedPtr<FBlueprintMessageTokenData> Data;
};

/**
 * Log message severity types
 */
UENUM()
enum class EBlueprintMessageSeverity : uint8
{
	CriticalError = 0,
	Error = 1,
	PerformanceWarning = 2,
	Warning = 3,
	Info = 4,
};

static_assert((int32)EBlueprintMessageSeverity::CriticalError == EMessageSeverity::CriticalError);
static_assert((int32)EBlueprintMessageSeverity::Error == EMessageSeverity::Error);
static_assert((int32)EBlueprintMessageSeverity::PerformanceWarning == EMessageSeverity::PerformanceWarning);
static_assert((int32)EBlueprintMessageSeverity::Warning == EMessageSeverity::Warning);
static_assert((int32)EBlueprintMessageSeverity::Info == EMessageSeverity::Info);

/**
 * Struct representing a message token factory registration
 */
struct BLUEPRINTMESSAGE_API FMessageTokenFactoryRegistration
{
	static const FName MD_TokenFactoryFunction;
	static const FName MD_TokenFactoryName;

	FMessageTokenFactoryRegistration() = default;
	FMessageTokenFactoryRegistration(UFunction* Function);

	/**
	 * Does the function matches factory standard.
	 *
	 * Requirements:
	 * - Native
	 * - Static
	 * - Pure
	 * - Returns FBlueprintMessageToken
	 */
	static bool IsTokenFactoryFunction(UFunction* Function);

	/**
	 *
	 */
	static void GetRegisteredFactories(TArray<FMessageTokenFactoryRegistration>& OutArray);

	friend bool operator==(const FMessageTokenFactoryRegistration& Lhs, const FMessageTokenFactoryRegistration& RHS)
	{
		return Lhs.FactoryClass == RHS.FactoryClass
			&& Lhs.FunctionName == RHS.FunctionName;
	}

public:
	/* */
	TWeakObjectPtr<UClass> FactoryClass;
	/* */
	FName FunctionName;
};
