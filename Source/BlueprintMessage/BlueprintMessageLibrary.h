// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintMessageToken.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintMessageLibrary.generated.h"

/**
 * Various global utility functions to interact with message log editor systems.
 */
UCLASS()
class BLUEPRINTMESSAGE_API UBlueprintMessageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Gather known message log categories from engine guts
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly=true))
	static TArray<FName> GetAvailableCategories();

	/**
	 * Opens the log for display to the user given certain conditions.
	 *
	 * @param LogCategory Log category name
	 * @param Severity Only messages of higher severity than this filter will be considered when checking.
	 * @param bForce Override the filter & log status & force the log to open.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Open Message Log", Category="Utilities|MessageLog", meta=(DevelopmentOnly=true))
	static void MessageLogOpen(
		UPARAM(DisplayName="Category", meta=(GetOptions="BlueprintMessage.BlueprintMessageLibrary.GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"),
		EBlueprintMessageSeverity Severity = EBlueprintMessageSeverity::Info,
		bool bForce = false);

	/**
	 * Notify the user with a message if there are messages present.
	 *
	 * @param Message The notification message.
	 * @param LogCategory Log category name
	 * @param Severity Only messages of higher severity than this filter will be considered when checking.
	 * @param bForce Notify anyway, even if the filters gives us no messages.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Notify Message Log", Category="Utilities|MessageLog", meta=(DevelopmentOnly=true))
	static void MessageLogNotify(
		FText Message = INVTEXT(""),
		UPARAM(DisplayName="Category", meta=(GetOptions="BlueprintMessage.BlueprintMessageLibrary.GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"),
		EBlueprintMessageSeverity Severity = EBlueprintMessageSeverity::Info,
		bool bForce = false);

	/**
	 * Clear messages in message log category.
	 *
	 * @param LogCategory Log category name
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Clear Message Log", Category="Utilities|MessageLog", meta=(DevelopmentOnly=true))
	static void MessageLogClear(
		UPARAM(DisplayName="Category", meta=(GetOptions="BlueprintMessage.BlueprintMessageLibrary.GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"));

	/**
	 * Copy text from message log category
	 *
	 * @param LogCategory Log category name
	 * @return Content of all messages in log category
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Copy Message Log", Category="Utilities|MessageLog", meta=(DevelopmentOnly=true))
	static FString MessageLogCopyAllMessages(
		UPARAM(DisplayName="Category", meta=(GetOptions="BlueprintMessage.BlueprintMessageLibrary.GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"));

};
