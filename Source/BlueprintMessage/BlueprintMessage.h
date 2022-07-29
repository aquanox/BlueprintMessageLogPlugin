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
UCLASS(BlueprintType)
class BLUEPRINTMESSAGE_API UBlueprintMessage : public UObject
{
	GENERATED_BODY()
public:

	/**
	 * Construct new message instance
	 *
	 * @param LogCategory message log category for this message
	 * @param Severity severity of this message
	 * @param Message initial message to add
	 * @returns message instance
	 */
	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(DevelopmentOnly))
	static UBlueprintMessage* CreateBlueprintMessage(UPARAM(DisplayName="Category") FName LogCategory = TEXT("BlueprintLog"), EBlueprintMessageSeverity Severity = EBlueprintMessageSeverity::Info, FText Message = INVTEXT(""));

	/**
	 * Construct new message instance from array of tokens
	 *
	 * @param Category message log category for this message
	 * @param Severity severity of this message
	 * @param Tokens array of tokens
	 * @returns message instance
	 */
	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(AutoCreateRefTerm="Tokens", LogCategory = "BlueprintLog", Severity="EBlueprintMessageSeverity::Info", DevelopmentOnly))
	static UBlueprintMessage* CreateBlueprintMessageFromTokens(FName Category, EBlueprintMessageSeverity Severity, TArray<FBlueprintMessageToken> Tokens);

	/* constructor */
	UBlueprintMessage();

	/* Add token to this message */
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Token"))
	void AddToken(const FBlueprintMessageToken& Token, FName Slot = TEXT(""));

	/* Add multiple tokens to this message */
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Tokens"))
	void AddTokens(const TArray<FBlueprintMessageToken>& Tokens);

	/* Clear all tokens in this message */
	UFUNCTION(BlueprintCallable)
	void ClearTokens();

	/* Show message in Message Log */
	UFUNCTION(BlueprintCallable, meta=(DevelopmentOnly))
	void Show();

	/* Show message in message Log and print it on screen */
	UFUNCTION(BlueprintCallable, meta=(DevelopmentOnly))
	void ShowAndPrint(bool bPrintToScreen = true, bool bPrintToLog = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f, const FName Key = NAME_None);

	/* Add a token slot with name */
	UFUNCTION(BlueprintCallable)
	void AddSlot(FName Name);

	/* Remove token slot with specified name */
	UFUNCTION(BlueprintCallable)
	void RemoveSlot(FName Name);

	/* Add token with text */
	UFUNCTION(BlueprintCallable)
	void AddTextToken(FText Value, FName Slot = TEXT(""));

	/* Add token with string */
	UFUNCTION(BlueprintCallable)
	void AddStringToken(FString Value, FName Slot = TEXT(""));

	/* Add token with name */
	UFUNCTION(BlueprintCallable)
	void AddNameToken(FName Value, FName Slot = TEXT(""));

	/* Add token with clickable URL */
	UFUNCTION(BlueprintCallable)
	void AddURLToken(FString Value, FText Label = INVTEXT(""), FName Slot = TEXT(""));

	/* Add token that overrides severity of message */
	UFUNCTION(BlueprintCallable)
	void AddSeverityToken(EBlueprintMessageSeverity Value, FName Slot = TEXT(""));

	/* Add token that links a specific object */
	UFUNCTION(BlueprintCallable)
	void AddObjectToken(UObject* Value, FText Label = INVTEXT(""), FName Slot = TEXT(""));

	/* Add asset reference from object */
	UFUNCTION(BlueprintCallable)
	void AddAssetToken(UObject* Value, FText Message = INVTEXT(""), FName Slot = TEXT(""));

	/* Add asset reference from soft path */
	UFUNCTION(BlueprintCallable)
	void AddAssetSoftPtrToken(TSoftObjectPtr<UObject> Value, FText Message = INVTEXT(""), FName Slot = TEXT(""));

	/* Add asset reference from soft path */
	UFUNCTION(BlueprintCallable)
	void AddAssetPathToken(FSoftObjectPath Value, FText Message = INVTEXT(""), FName Slot = TEXT(""));

	/* Add image from AppStyle by name */
	UFUNCTION(BlueprintCallable)
	void AddImageToken(FName Value, FName Slot = TEXT(""));

	/* Add actor reference */
	UFUNCTION(BlueprintCallable)
	void AddActorToken(AActor* Value, FText Message = INVTEXT(""), FName Slot = TEXT(""));

	/* add token that activates specific tutorial */
	UFUNCTION(BlueprintCallable)
	void AddTutorialToken(FString TutorialAssetName, FName Slot = TEXT(""));

	/* add token with documentation link */
	UFUNCTION(BlueprintCallable)
	void AddDocumentationToken(FString DocumentationLink, FName Slot = TEXT(""));

	/* Add token with dynamically generated text */
	// UFUNCTION(BlueprintCallable)
	void AddDynamicTextToken_Delegate(FGetMessageDynamicText Value, FName Slot = TEXT(""));
	/* Add token with dynamically generated text */
	// UFUNCTION(BlueprintCallable)
	void AddDynamicTextToken_Function(UObject* Object, FName FunctionName, FName Slot = TEXT(""));


protected:

	TSharedRef<FTokenizedMessage> BuildMessage() const;

	UPROPERTY()
	FName Category;
	UPROPERTY()
	EBlueprintMessageSeverity Severity;
	UPROPERTY()
	FText InitialMessage;
	UPROPERTY()
	TArray<FBlueprintMessageToken> Tokens;
};
