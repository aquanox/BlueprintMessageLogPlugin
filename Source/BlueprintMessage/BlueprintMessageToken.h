// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

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

	FBlueprintMessageToken(FName InSlot) : Name(InSlot)
	{
	}

	FBlueprintMessageToken(TSharedRef<class IMessageToken> InToken) : Data(InToken)
	{
	}

	bool operator==(const FName& Other) const { return Name == Other; }

	FName Name;

	TSharedPtr<class IMessageToken> Data;

	static const FBlueprintMessageToken EMPTY_TOKEN;
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
 * Token factory
 */
UCLASS()
class BLUEPRINTMESSAGE_API UBlueprintMessageTokenFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeSlotToken(FName Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeTextToken(FText Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeStringToken(FString Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeNameToken(FName Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeUrlToken(FString Value, FText Label = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeSeverityToken(EBlueprintMessageSeverity Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeObjectToken(UObject* Value, FText Label = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeAssetToken(UObject* Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeAssetSoftPtrToken(TSoftObjectPtr<UObject> Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeClassSoftPtrToken(TSoftClassPtr<UObject> Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeClassPathToken(FSoftClassPath Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeAssetPathToken(FSoftObjectPath Value, FText Message = INVTEXT(""));

	static FBlueprintMessageToken MakeAssetPathToken_Internal(FString AssetPath, FText Message);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeImageToken(FName Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeActorToken(AActor* Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeTutorialToken(FString Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeDocumentationToken(FString Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeDynamicTextToken_Delegate(FGetMessageDynamicText Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeDynamicTextToken_Function(UObject* Object, FName FunctionName);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeActionToken(FText Name, FText Description, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse = false);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken MakeEditorUtilityWidgetToken(UPARAM(meta=(AllowedClasses="EditorUtilityWidgetBlueprint")) TSoftObjectPtr<UBlueprint> Widget, FText ActionName, FText Description);

};
