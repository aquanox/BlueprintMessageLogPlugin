// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintMessageToken.h"
#include "UObject/Object.h"
#include "BlueprintMessage.generated.h"

/**
 * This class represents a rich tokenized message template,
 * such as would be used for compiler output with 'hyperlinks' to source file locations
 *
 */
UCLASS(BlueprintType, Transient, Category="Utilities|MessageLog")
class BLUEPRINTMESSAGE_API UBlueprintMessage : public UObject
{
	GENERATED_BODY()
public:

	/**
	 * Construct new message instance.
	 *
	 * @param LogCategory message log category for this message
	 * @param Severity severity of this message
	 * @returns message instance
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Create Log Message", Category="Utilities|MessageLog", meta=(BlueprintInternalUseOnly=true))
	static UBlueprintMessage* CreateBlueprintMessage(
		UPARAM(DisplayName="Category", meta=(GetOptions="GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"),
		EBlueprintMessageSeverity Severity = EBlueprintMessageSeverity::Info);

	/**
	 * Construct new message instance with initial text.
	 *
	 * @param LogCategory message log category for this message
	 * @param Severity severity of this message
	 * @param Message initial message to add
	 * @param bShow trigger show right after creation
	 * @returns message instance
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Create Simple Log Message", Category="Utilities|MessageLog", meta=(AdvancedDisplay=3))
	static UBlueprintMessage* CreateSimpleBlueprintMessage(
		UPARAM(DisplayName="Category", meta=(GetOptions="GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"),
		EBlueprintMessageSeverity Severity = EBlueprintMessageSeverity::Info,
		FText Message = INVTEXT(""),
		bool bShow = false);

	/**
	 * Opens the log for display to the user given certain conditions.
	 *
	 * @param LogCategory Log category name
	 * @param Severity Only messages of higher severity than this filter will be considered when checking.
	 * @param bForce Override the filter & log status & force the log to open.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Open Message Log", Category="Utilities|MessageLog")
	static void MessageLogOpen(
		UPARAM(DisplayName="Category", meta=(GetOptions="GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"),
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
	UFUNCTION(BlueprintCallable, DisplayName="Notify Message Log", Category="Utilities|MessageLog")
	static void MessageLogNotify(
		FText Message = INVTEXT(""),
		UPARAM(DisplayName="Category", meta=(GetOptions="GetAvailableCategories")) FName LogCategory = TEXT("BlueprintLog"),
		EBlueprintMessageSeverity Severity = EBlueprintMessageSeverity::Info,
		bool bForce = false);

public:

	/* constructor */
	UBlueprintMessage();

	/**
	 * Categories
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly=true))
	static TArray<FName> GetAvailableCategories();

	/**
	 * Duplicate message and return its copy
	 */
	UFUNCTION(BlueprintCallable, Category="Utilities|MessageLog")
	UPARAM(DisplayName="Message") UBlueprintMessage* Duplicate();

	/* Add token to this message */
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Token"), Category="Utilities|MessageLog")
	UPARAM(DisplayName="Message") UBlueprintMessage* AddToken(const FBlueprintMessageToken& Token, FName Slot = NAME_None);

	/* Add multiple tokens to this message */
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Tokens"), Category="Utilities|MessageLog")
	UPARAM(DisplayName="Message") UBlueprintMessage* AddTokens(const TArray<FBlueprintMessageToken>& Tokens);

	/* Clear all tokens in this message */
	UFUNCTION(BlueprintCallable, Category="Utilities|MessageLog")
	UPARAM(DisplayName="Message") UBlueprintMessage* ClearTokens();

	/*
	 * Show message in Message Log
	 *
	 * @param bOpenLog Open Message Log window
	 */
	UFUNCTION(BlueprintCallable, Category="Utilities|MessageLog", meta=(AdvancedDisplay=0))
	void Show();

	/* Show message in message Log and print it on screen */
	UFUNCTION(BlueprintCallable, Category="Utilities|MessageLog")
	void ShowAndPrint(bool bPrintToScreen = true, bool bPrintToLog = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f, const FName Key = NAME_None);

	/* Add a token slot with name */
	UFUNCTION(BlueprintCallable, Category="Utilities|MessageLog")
	UPARAM(DisplayName="Message") UBlueprintMessage* AddNamedSlot(FName Name);

	/* Remove token slot with specified name */
	UFUNCTION(BlueprintCallable, Category="Utilities|MessageLog")
	UPARAM(DisplayName="Message") UBlueprintMessage* RemoveNamedSlot(FName Name);

	/** Change severity for this message */
	UFUNCTION(BlueprintCallable, Category="Utilities|MessageLog")
	UPARAM(DisplayName="Message") UBlueprintMessage* SetSeverity(EBlueprintMessageSeverity Severity);

protected:

	TSharedRef<FTokenizedMessage> BuildMessage() const;
	void ShowImpl(const FName& InCategory, const TSharedRef<FTokenizedMessage>& InMessage) const;

	UPROPERTY()
	FName Category = TEXT("BlueprintLog");
	UPROPERTY()
	EBlueprintMessageSeverity Severity = EBlueprintMessageSeverity::Info;
	UPROPERTY()
	FText InitialMessage;
	/** Stream of tokens for this message */
	UPROPERTY()
	TArray<FBlueprintMessageToken> Tokens;
	/** Should we mirror message log messages from this instance to the output log? */
	UPROPERTY(BlueprintReadWrite, Category=Message, meta=(AllowPrivateAccess))
	bool bSuppressLoggingToOutputLog = false;
};

DECLARE_LOG_CATEGORY_EXTERN(LogBlueprintMessage, Log, All);
