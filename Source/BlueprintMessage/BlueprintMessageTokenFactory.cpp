// Copyright 2022, Aquanox.

#include "BlueprintMessageTokenFactory.h"
#include "Misc/UObjectToken.h"

#if WITH_EDITOR
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#endif

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

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeAssetToken(UObject* Value, FText Message)
{
	if (UClass* const AsClass = Cast<UClass>(Value))
	{
		return MakeClassPathToken(FSoftClassPath(AsClass), Message);
	}
	else
	{
		return MakeAssetPathToken(FSoftObjectPath(Value), Message);
	}
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeAssetSoftPtrToken(TSoftObjectPtr<UObject> Value, FText Message)
{
	return MakeAssetPathToken(Value.ToSoftObjectPath(), Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeClassSoftPtrToken(TSoftClassPtr<UObject> Value, FText Message)
{
	return MakeAssetPathToken(Value.ToSoftObjectPath(), Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeClassPathToken(FSoftClassPath Value, FText Message)
{
	return MakeAssetPathToken(Value, Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeAssetPathToken(FSoftObjectPath Value, FText Message)
{
	FString InAssetName = !Value.IsNull() ? Value.ToString() : TEXT("Unknown");
	return MakeAssetPathToken_Internal(InAssetName, Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeAssetPathToken_Internal(FString AssetPath, FText Message)
{
	Message = Message.IsEmpty() ? FText::FromString(FPackageName::GetShortName(AssetPath)) : Message;
	return FBlueprintMessageToken(FAssetNameToken::Create(AssetPath, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeImageToken(FName Value)
{ // FAppStyle::Get().GetBrush(ImageToken->GetImageName())
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

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeTutorialToken(FString Value)
{
	return FBlueprintMessageToken(FTutorialToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDocumentationToken(FString Value)
{ // Engine/Animation/AnimBlueprints/AnimGraph
	return FBlueprintMessageToken(FDocumentationToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDynamicTextToken_Delegate(FGetMessageDynamicText Value)
{
	return FBlueprintMessageToken(FDynamicTextToken::Create(MakeAttributeLambda([Value]()
	{
		return Value.IsBound() ? Value.Execute() : FText::GetEmpty();
	})));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeDynamicTextToken_Function(UObject* Object, FName FunctionName)
{
	TAttribute<FText> Attribute;
	Attribute.BindUFunction(Object, FunctionName);
	return FBlueprintMessageToken(FDynamicTextToken::Create(Attribute));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeActionToken(FText Name, FText Description, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse)
{
	return FBlueprintMessageToken(
		FActionToken::Create(Name, Description, FOnActionTokenExecuted::CreateLambda([Action]()
		{
			if (Action.IsBound())
			{
				Action.Execute();
			}
		}), bInSingleUse)
	);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::MakeEditorUtilityWidgetToken(TSoftObjectPtr<UBlueprint> Widget, FText ActionName, FText Description)
{
#if WITH_EDITOR
	const FString WidgetAssetName = Widget.GetAssetName();

	ActionName = !(ActionName.IsEmpty()) ? ActionName
				: !WidgetAssetName.IsEmpty() ? FText::FromString(WidgetAssetName) : INVTEXT("Action");

	return FBlueprintMessageToken(
		FActionToken::Create(ActionName, Description, FOnActionTokenExecuted::CreateLambda([Widget]()
		{
			if (UObject* WidgetClass = Widget.LoadSynchronous())
			{
				if (UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(WidgetClass))
				{
					GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>()->SpawnAndRegisterTab(EditorWidget);
				}
			}
		}), false)
	);
#else
	return FBlueprintMessageToken();
#endif
}
