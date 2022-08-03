// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintMessageToken.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintMessageTokenFactory.generated.h"

/**
 * Message token factory
 */
UCLASS()
class BLUEPRINTMESSAGE_API UBlueprintMessageTokenFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeTextToken(FText Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeStringToken(FString Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeNameToken(FName Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeUrlToken(FString Value, FText Label = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeObjectToken(UObject* Value, FText Label = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeAssetToken(UObject* Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeAssetSoftPtrToken(TSoftObjectPtr<UObject> Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeClassSoftPtrToken(TSoftClassPtr<UObject> Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeClassPathToken(FSoftClassPath Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeAssetPathToken(FSoftObjectPath Value, FText Message = INVTEXT(""));

	static FBlueprintMessageToken MakeAssetPathToken_Internal(FString AssetPath, FText Message);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeImageToken(FName Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeActorToken(AActor* Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeTutorialToken(FString Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeDocumentationToken(FString Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeDynamicTextToken_Delegate(FGetMessageDynamicText Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeDynamicTextToken_Function(UObject* Object, FName FunctionName);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeActionToken(FText Name, FText Description, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse = false);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe, TokenFactoryFunction))
	static FBlueprintMessageToken MakeEditorUtilityWidgetToken(UPARAM(meta=(AllowedClasses="EditorUtilityWidgetBlueprint")) TSoftObjectPtr<UBlueprint> Widget, FText ActionName, FText Description);

};
