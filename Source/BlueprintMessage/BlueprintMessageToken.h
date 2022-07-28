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
	static FBlueprintMessageToken CreateSlotToken(FName Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateTextToken(FText Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateStringToken(FString Value);

	UFUNCTION(BlueprintPure, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateNameToken(FName Value);

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateDynamicText(FGetMessageDynamicText Value);

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateUrlToken(FString Value, FText DisplayText = INVTEXT(""));

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateSeverityToken(EBlueprintMessageSeverity Value);

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateObjectToken(UObject* Value, FText Label = INVTEXT(""));

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateAssetToken(UObject* Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateAssetPathToken(FSoftObjectPath Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateImageToken(FName Value);

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateActorToken(AActor* Value, FText Message = INVTEXT(""));

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateTutorialToken(FString TutorialAssetName);

	UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateDocumentationToken(FString DocumentationLink);

	//UFUNCTION(BlueprintCallable, Category="Utilities|BlueprintMessage", meta=(BlueprintThreadSafe))
	static FBlueprintMessageToken CreateActionToken(FText ActionName, FText ActionDescription, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse = false);
};
