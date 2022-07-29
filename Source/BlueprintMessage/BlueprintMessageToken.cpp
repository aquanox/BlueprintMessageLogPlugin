// Copyright 2022, Aquanox.

#include "BlueprintMessageToken.h"
#include "Logging/TokenizedMessage.h"
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

const FBlueprintMessageToken FBlueprintMessageToken::EMPTY_TOKEN;

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateSlotToken(FName Value)
{
	return FBlueprintMessageToken(Value);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateTextToken(FText Value)
{
	return FBlueprintMessageToken(FTextToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateStringToken(FString Value)
{
	return FBlueprintMessageToken(FTextToken::Create(FText::FromString(Value)));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateNameToken(FName Value)
{
	return FBlueprintMessageToken(FTextToken::Create(FText::FromName(Value)));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateUrlToken(FString Value, FText Message)
{
	return FBlueprintMessageToken(FURLToken::Create(Value, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateSeverityToken(EBlueprintMessageSeverity Value)
{
	return FBlueprintMessageToken(FSeverityToken::Create(static_cast<EMessageSeverity::Type>(Value)));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateObjectToken(UObject* Value, FText Label)
{
	return FBlueprintMessageToken(FUObjectToken::Create(Value, Label));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateAssetToken(UObject* Value, FText Message)
{
	if (UClass* const AsClass = Cast<UClass>(Value))
	{
		return CreateClassPathToken(FSoftClassPath(AsClass), Message);
	}
	else
	{
		return CreateAssetPathToken(FSoftObjectPath(Value), Message);
	}
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateAssetSoftPtrToken(TSoftObjectPtr<UObject> Value, FText Message)
{
	return CreateAssetPathToken(Value.ToSoftObjectPath(), Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateClassSoftPtrToken(TSoftClassPtr<UObject> Value, FText Message)
{
	return CreateAssetPathToken(Value.ToSoftObjectPath(), Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateClassPathToken(FSoftClassPath Value, FText Message)
{
	return CreateAssetPathToken(Value, Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateAssetPathToken(FSoftObjectPath Value, FText Message)
{
	FString InAssetName = !Value.IsNull() ? Value.ToString() : TEXT("Unknown");
	return CreateAssetPathToken_Internal(InAssetName, Message);
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateAssetPathToken_Internal(FString AssetPath, FText Message)
{
	Message = Message.IsEmpty() ? FText::FromString(FPackageName::GetShortName(AssetPath)) : Message;
	return FBlueprintMessageToken(FAssetNameToken::Create(AssetPath, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateImageToken(FName Value)
{ // FAppStyle::Get().GetBrush(ImageToken->GetImageName())
	return FBlueprintMessageToken(FImageToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateActorToken(AActor* Value, FText Message)
{
	FString ActorPath = Value ? Value->GetPathName() : FString();
	FGuid Guid;
	Message = Message.IsEmpty() && Value ? FText::FromString(Value->GetActorNameOrLabel()) : Message;
#if WITH_EDITOR
	Guid = Value ? Value->GetActorGuid() : FGuid();
#endif
	return FBlueprintMessageToken(FActorToken::Create(ActorPath, Guid, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateTutorialToken(FString Value)
{
	return FBlueprintMessageToken(FTutorialToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateDocumentationToken(FString Value)
{ // Engine/Animation/AnimBlueprints/AnimGraph
	return FBlueprintMessageToken(FDocumentationToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateDynamicTextToken_Delegate(FGetMessageDynamicText Value)
{
	return FBlueprintMessageToken(FDynamicTextToken::Create(MakeAttributeLambda([Value]()
	{
		return Value.IsBound() ? Value.Execute() : FText::GetEmpty();
	})));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateDynamicTextToken_Function(UObject* Object, FName FunctionName)
{
	TAttribute<FText> Attribute;
	Attribute.BindUFunction(Object, FunctionName);
	return FBlueprintMessageToken(FDynamicTextToken::Create(Attribute));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateActionToken(FText Name, FText Description, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse)
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

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateEditorUtilityWidgetToken(TSoftObjectPtr<UBlueprint> Widget, FText Description)
{
#if WITH_EDITOR
	FText ActionName = FText::FromString(Widget.GetAssetName());

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
	return FBlueprintMessageToken::EMPTY_TOKEN;
#endif
}
