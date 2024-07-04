// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintMessageToken.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "BlueprintMessageTokenFactory.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(FText, FGetMessageDynamicText);
DECLARE_DYNAMIC_DELEGATE_RetVal(FString, FGetMessageDynamicString);

DECLARE_DYNAMIC_DELEGATE(FBlueprintMessageActionDelegate);

/**
 * Message token factory
 */
UCLASS()
class BLUEPRINTMESSAGE_API UBlueprintMessageTokenFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * @brief Create a token that represents a simple text
	 * @param Value Token text
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeTextToken(FText Value);

	/**
	 * @brief Create a token that represents a simple text
	 * @param Value Token text
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeStringToken(FString Value);

	/**
	 * @brief Create a token that represents a simple text
	 * @param Value Token text
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeNameToken(FName Value);

	/**
	 * @brief Create a token that represents a hyperlink that will be opened on activation
	 * @param Value Hyperlink to external resource
	 * @param Label Hyperlink label text
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeUrlToken(FString Value, FText Label = INVTEXT("Link"));

	/**
	 * @brief Create a token that represents a UObject reference.
	 *        Some editors would navigate to specific objects upon interaction
	 *
	 * @param Value Object of interest
	 * @param Label Object label text
	 * @return Token instance
	 *
	 * @see FUnrealEdMisc::OnMessageTokenActivated
	 * @see FUnrealEdMisc::OnGetDisplayName
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeObjectToken(UObject* Value, FText Label = INVTEXT(""));

	/**
	 * @brief Token that represents an asset reference.
	 * @param Value Asset Strong Object Reference
	 * @param Label Asset Label (Asset name is used if empty)
	 * @return Token instance
	 *
	 * @note This creates hard references. Use with caution, or when need to get asset from object instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeAssetToken(UObject* Value, FText Label = INVTEXT(""));

	/**
	 * @brief Token that represents an asset reference.
	 * @param Value Asset Soft Object Reference
	 * @param Label Asset Label (Asset name is used if empty)
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeSoftAssetToken(TSoftObjectPtr<UObject> Value, FText Label = INVTEXT(""));

	/**
	 * @brief Token that represents an asset reference.
	 * @param Value Asset Soft Class Reference
	 * @param Label Asset Label (Asset name is used if empty)
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeSoftClassToken(TSoftClassPtr<UObject> Value, FText Label = INVTEXT(""));

	/**
	 * @brief Token that represents an asset reference.
	 * @param Value Asset Soft Object Path
	 * @param Label Asset Label (Asset name is used if empty)
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeSoftAssetPathToken(FSoftObjectPath Value, FText Label = INVTEXT(""));

	/**
	 * @brief Token that represents an asset reference.
	 * @param Value Asset Soft Class Path
	 * @param Label Asset Label (Asset name is used if empty)
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeSoftClassPathToken(FSoftClassPath Value, FText Label = INVTEXT(""));

	/**
	 * @brief Token that represents an asset reference.
	 * @param Value Path to asset
	 * @param Label Asset Label (Asset name is used if empty)
	 * @return Token instance
	 *
	 * @see FUnrealEdMisc::OnGotoAsset
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeAssetPathToken(FString Value, FText Label = INVTEXT(""));

	/**
	 * @brief Create a token that represents an image that will be shown in log message
	 * @param Value Image brush path
	 * @return Token instance
	 *
	 * @see FAppStyle::Get()
	 * @see ISlateStyle::GetBrush()
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeImageToken(FName Value);

	/**
	 * @brief Create a token that represents an Actor reference.
	 *        Some editors would navigate to specific objects upon interaction.
	 * @param Value Actor of interest
	 * @param Message Actor label text (or Actor Name if empty)
	 * @return Token instance
	 *
	 * @see FUnrealEdMisc::OnActorTokenActivated
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeActorToken(AActor* Value, FText Message = INVTEXT(""));

	/**
	 * @brief Create a token that represents a Tutorial Asset reference.
	 *        Would launch one upon activation.
	 * @param Value EditorTutorial asset
	 * @return Token instance
	 *
	 * @note SClassViewer does not support AllowedClasses or MetaClass as need filter EditorTutorial
	 *       SAssetViewer does not support MetaClass as Object would be Blueprint but need filter EditorTutorial
	 *		 Path properties do not have dropdowns
	 *
	 * @note If tutorials are disabled nothing will happen after clicking
	 *
	 * @see SMessageLogMessageListRow::HandleTutorialHyperlinkNavigate
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeTutorialToken(UPARAM(meta=(MetaClass="EditorTutorial")) TSoftObjectPtr<UBlueprint> Value);

	/**
	 * @brief Create a token that represents a URL to Unreal Engine Documentation
	 * @param Value Documentation path, i.e. "Engine/Animation/AnimBlueprints/AnimGraph"
	 * @return Token instance
	 *
	 * SMessageLogMessageListRow::HandleDocsHyperlinkNavigate
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeDocumentationToken(FString Value);

	/**
	 * Create token with a dynamic text
	 * @param Value text source callback
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeDynamicTextToken_Delegate(FGetMessageDynamicText Value);

	/**
	 * Create token with a dynamic text
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeDynamicTextToken_Function(UObject* Object, FName FunctionName);

	/**
	 * @brief Create a token that represents a button that would invoke a generic delegate.
	 *        Delegate owner must exist.
	 * @param Name Button name text
	 * @param Description Button tooltip text
	 * @param Action Delegate to invoke
	 * @param bInSingleUse Should hide button after clicking once?
	 * @return Token instance
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeActionToken(FText Name, FText Description, UPARAM(DisplayName="Event") const FBlueprintMessageActionDelegate& Action, bool bInSingleUse = false);

	/**
	 * @brief Create a token that represents a button that would spawn an Editor Utility Widget
	 * @param Widget Blutility Widget asset
	 * @param ActionName Button label text (default is Asset Name)
	 * @param Description Button label tooltip (default is None)
	 * @param bSingleUse Should hide button after clicking once?
	 * @return Token instance
	 *
	 * @note I wish SAssetPicker had MetaClass for Blueprints
	 */
	UFUNCTION(BlueprintPure, Category="Utilities|MessageLog|Factory", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeEditorUtilityWidgetToken(UPARAM(meta=(AllowedClasses="EditorUtilityWidgetBlueprint")) TSoftObjectPtr<UBlueprint> Widget, FText ActionName = INVTEXT(""), FText Description = INVTEXT(""), bool bSingleUse = false);

};
