// Copyright 2022, Aquanox.

#include "BlueprintMessageTokenFactory.h"

#include "BlueprintMessageHelpers.h"
#include "Misc/UObjectToken.h"



// IMessageToken
//  FTextToken | implemented
//  FDynamicTextToken | implemented
//  FURLToken | implemented
//  FSeverityToken | implemented
//  FUObjectToken | implemented
//  FActorToken | implemented
//  FAssetNameToken | implemented
//  FImageToken | implemented
//  FTutorialToken | implemented
//  FActionToken | special
//  FDocumentationToken | implemented
//  FEdGraphToken | special
//  FNiagaraCompileEventToken | special

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
	return FBlueprintMessageToken(FURLToken::Create(Value, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeObjectToken(UObject* Value, FText Label)
{
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
	//Label = Label.IsEmpty() ? FText::FromString(FPackageName::GetShortName(AssetPath)) : Label;
	return FBlueprintMessageToken(FAssetNameToken::Create(AssetPath, Label));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeImageToken(FName Value)
{
	return FBlueprintMessageToken(FImageToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeActorToken(AActor* Value, FText Message)
{
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
	return FBlueprintMessageToken(FTutorialToken::Create(Value.GetLongPackageName()));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDocumentationToken(FString Value)
{
	return FBlueprintMessageToken(FDocumentationToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDynamicTextToken_Delegate(FGetMessageDynamicText Value)
{
	TAttribute<FText> Attribute;
	if (Value.IsBound())
	{
		Attribute.BindUFunction(Value.GetUObject(), Value.GetFunctionName());
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
	return FBlueprintMessageToken(FDynamicTextToken::Create(Attribute));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeActionToken(FText Name, FText Description, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse)
{
	return FBlueprintMessageToken(
		FActionToken::Create(Name, Description,
			FOnActionTokenExecuted::CreateStatic(FBlueprintMessageHelpers::InvokeDelegate, Action),
			bInSingleUse)
	);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeEditorUtilityWidgetToken(TSoftObjectPtr<UBlueprint> Widget, FText ActionName, FText Description, bool bSingleUse)
{
	const FString WidgetAssetName = Widget.GetAssetName();

	ActionName = !ActionName.IsEmpty() ? ActionName
				: !WidgetAssetName.IsEmpty() ? FText::FromString(WidgetAssetName) : INVTEXT("Action");

	return FBlueprintMessageToken(
		FActionToken::Create(ActionName, Description,
			FOnActionTokenExecuted::CreateStatic(FBlueprintMessageHelpers::SpawnEditorUtilityWidget, Widget),
			bSingleUse)
	);
}
