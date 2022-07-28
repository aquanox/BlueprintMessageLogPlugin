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

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateDynamicText(FGetMessageDynamicText Value)
{
	TAttribute<FText> Attribute;
	Attribute.BindUFunction(Value.GetUObject(), Value.GetFunctionName());
	return FBlueprintMessageToken(FDynamicTextToken::Create(Attribute));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateUrlToken(FString Value, FText DisplayText)
{
	return FBlueprintMessageToken(FURLToken::Create(Value, DisplayText));
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
	FString InAssetName = Value ? Value->GetPathName() : TEXT("Unknown");
	Message = Message.IsEmpty() ? FText::FromString(GetNameSafe(Value)) : Message;
	return FBlueprintMessageToken(FAssetNameToken::Create(InAssetName, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateAssetPathToken(FSoftObjectPath Value, FText Message)
{
	FString InAssetName = !Value.IsNull() ? Value.ToString() : TEXT("Null");
	Message = Message.IsEmpty() ? FText::FromString(Value.GetAssetName()) : Message;
	return FBlueprintMessageToken(FAssetNameToken::Create(InAssetName, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateImageToken(FName Value)
{ // FAppStyle::Get().GetBrush(ImageToken->GetImageName())
	return FBlueprintMessageToken(FImageToken::Create(Value));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateActorToken(AActor* Value, FText Message)
{
	FString ActorPath = Value ? Value->GetPathName() : FString();
	FGuid Guid = Value ? Value->GetActorGuid() : FGuid();
	Message = Message.IsEmpty() && Value ? FText::FromString(Value->GetActorNameOrLabel()) : Message;
	return FBlueprintMessageToken(FActorToken::Create(ActorPath, Guid, Message));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateTutorialToken(FString TutorialAssetName)
{
	return FBlueprintMessageToken(FTutorialToken::Create(TutorialAssetName));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateDocumentationToken(FString DocumentationLink)
{ // Engine/Animation/AnimBlueprints/AnimGraph
	return FBlueprintMessageToken(FDocumentationToken::Create(DocumentationLink));
}

FBlueprintMessageToken UBlueprintMessageTokenFactory::CreateActionToken(FText ActionName, FText ActionDescription, const FBlueprintMessageActionDelegate& Action, bool bInSingleUse)
{
	return FBlueprintMessageToken(FActionToken::Create(ActionName, ActionDescription, nullptr, bInSingleUse));
}
