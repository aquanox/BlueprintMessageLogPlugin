// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "Logging/TokenizedMessage.h"
#include "BlueprintMessageToken.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(FText, FGetMessageDynamicText);
DECLARE_DYNAMIC_DELEGATE_RetVal(FText, FGetMessageDynamicString);
DECLARE_DYNAMIC_DELEGATE(FBlueprintMessageActionDelegate);

/**
 * Wrapper-container for generic message token to pass around in blueprint
 */
USTRUCT(BlueprintType, meta=(HiddenByDefault, DisableSplitPin))
struct BLUEPRINTMESSAGE_API FBlueprintMessageToken
{
	GENERATED_BODY()

	FBlueprintMessageToken() = default;
	FBlueprintMessageToken(FName InSlot);
	FBlueprintMessageToken(TSharedRef<IMessageToken> InToken);

	bool operator==(const FName& Other) const { return Name == Other; }

	/* Token name */
	FName Name;
	/* Token instance */
	TSharedPtr<IMessageToken> Data;
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

UENUM(BlueprintType)
enum class EBlueprintMessageShowMode : uint8
{
	/**
	 * @brief Do nothing
	 */
	Default,
	/**
	 * @brief Open message log window if messages are available
	 */
	Open,
	/**
	 * @brief Always open message log
	 */
	OpenAlways
};

/**
 * Struct representing a message token factory registration
 */
struct BLUEPRINTMESSAGE_API FMessageTokenFactoryRegistration
{
	TWeakObjectPtr<UClass> FactoryClass;
	FName FunctionName;

	FMessageTokenFactoryRegistration() = default;
	FMessageTokenFactoryRegistration(UFunction* Function);

	static const FName MD_TokenFactoryFunction;
	static const FName MD_TokenFactoryName;

	/**
	 * Does the function matches factory standard.
	 *
	 * Requirements:
	 * - Native
	 * - Static
	 * - Pure
	 * - Returns FBlueprintMessageToken
	 */
	static bool IsTokenFactoryFunction(UFunction& Function);

	/**
	 *
	 */
	static void GetRegisteredFactories(TArray<FMessageTokenFactoryRegistration>& OutArray);

	friend bool operator==(const FMessageTokenFactoryRegistration& Lhs, const FMessageTokenFactoryRegistration& RHS)
	{
		return Lhs.FactoryClass == RHS.FactoryClass
			&& Lhs.FunctionName == RHS.FunctionName;
	}
};
