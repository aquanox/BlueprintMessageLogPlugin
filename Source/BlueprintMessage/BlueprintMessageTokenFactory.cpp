// Copyright 2022, Aquanox.

#include "BlueprintMessageTokenFactory.h"
#include "BlueprintMessage.h"
#include "BlueprintMessageHelpers.h"
#include "Misc/UObjectToken.h"

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeTextToken(FText Value)
{
	return FBlueprintMessageToken(FTextToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeStringToken(FString Value)
{
	return FBlueprintMessageToken(FTextToken::Create(FText::FromString(Value)));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeNameToken(FName Value)
{
	return FBlueprintMessageToken(FTextToken::Create(FText::FromName(Value)));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeUrlToken(FString Value, FText Message)
{
	if (Value.IsEmpty())
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("URLToken was created with empty value"));
	}

	return FBlueprintMessageToken(FURLToken::Create(Value, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeObjectToken(UObject* Value, FText Label)
{
	if (!IsValid(Value))
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("ObjectToken was created with empty value"));
	}

	return FBlueprintMessageToken(FUObjectToken::Create(Value, Label));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeAssetToken(UObject* Value, FText Label)
{
	if (UClass* const AsClass = Cast<UClass>(Value))
	{
		return MakeSoftClassPathToken(FSoftClassPath(AsClass), Label);
	}
	else
	{
		return MakeSoftAssetPathToken(FSoftObjectPath(Value), Label);
	}
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeSoftAssetToken(TSoftObjectPtr<UObject> Value, FText Label)
{
	return MakeAssetPathToken(Value.ToSoftObjectPath().ToString(), Label);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeSoftClassToken(TSoftClassPtr<UObject> Value, FText Label)
{
	return MakeAssetPathToken(Value.ToSoftObjectPath().ToString(), Label);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeSoftClassPathToken(FSoftClassPath Value, FText Label)
{
	return MakeAssetPathToken(Value.ToString(), Label);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeSoftAssetPathToken(FSoftObjectPath Value, FText Label)
{
	return MakeAssetPathToken(Value.ToString(), Label);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeAssetPathToken(FString AssetPath, FText Label)
{
	if (AssetPath.IsEmpty())
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("AssetPathToken was created with empty value"));
	}

	return FBlueprintMessageToken(FAssetNameToken::Create(AssetPath, Label));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeImageToken(FName Value)
{
	if (Value.IsNone())
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("ImageToken was created with empty value"));
	}

	return FBlueprintMessageToken(FImageToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeActorToken(AActor* Value, FText Message)
{
	if (!IsValid(Value))
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("ActorToken was created with empty value"));
	}

	FString ActorPath = Value ? Value->GetPathName() : FString();
	FGuid Guid;
	Message = Message.IsEmpty() && Value ? FText::FromString(Value->GetActorNameOrLabel()) : Message;
#if WITH_EDITOR
	Guid = Value ? Value->GetActorGuid() : FGuid();
#endif
	return FBlueprintMessageToken(FActorToken::Create(ActorPath, Guid, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeTutorialToken(TSoftObjectPtr<UBlueprint> Value)
{
	if (Value.IsNull())
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("TutorialToken was created with empty value"));
	}

	return FBlueprintMessageToken(FTutorialToken::Create(Value.GetLongPackageName()));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDocumentationToken(FString Value)
{
	if (Value.IsEmpty())
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("DocumentationToken was created with empty value"));
	}

	return FBlueprintMessageToken(FDocumentationToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDynamicTextToken_Delegate(FGetMessageDynamicText Value)
{
	TAttribute<FText> Attribute;
	if (Value.IsBound())
	{
		Attribute.BindUFunction(Value.GetUObject(), Value.GetFunctionName());
	}
	else
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("DynamicTextToken was created with empty value"));
	}
	return FBlueprintMessageToken(FDynamicTextToken::Create(Attribute));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDynamicTextToken_Function(UObject* Object, FName FunctionName)
{
	TAttribute<FText> Attribute;
	if (IsValid(Object) && !FunctionName.IsNone())
	{
		Attribute.BindUFunction(Object, FunctionName);
	}
	else
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("DynamicTextToken was created with empty value"));
	}
	return FBlueprintMessageToken(FDynamicTextToken::Create(Attribute));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeActionToken(FText Name, FText Description, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse)
{
	if (!Action.IsBound())
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("ActionToken was created with empty value"));
	}

	return FBlueprintMessageToken(
		FActionToken::Create(Name, Description,
			FOnActionTokenExecuted::CreateStatic(FBlueprintMessageHelpers::InvokeDynamicDelegate, Action),
			bInSingleUse)
	);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeEditorUtilityWidgetToken(TSoftObjectPtr<UBlueprint> Widget, FText ActionName, FText Description, bool bSingleUse)
{
	if (Widget.IsNull())
	{
		UE_LOG(LogBlueprintMessage, Warning, TEXT("EditorUtilityWidgetToken was created with empty value"));
	}

	const FString WidgetAssetName = Widget.GetAssetName();

	ActionName = !ActionName.IsEmpty() ? ActionName
				: !WidgetAssetName.IsEmpty() ? FText::FromString(WidgetAssetName) : INVTEXT("Action");

	return FBlueprintMessageToken(
		FActionToken::Create(ActionName, Description,
			FOnActionTokenExecuted::CreateStatic(FBlueprintMessageHelpers::SpawnEditorUtilityWidget, Widget),
			bSingleUse)
	);
}
