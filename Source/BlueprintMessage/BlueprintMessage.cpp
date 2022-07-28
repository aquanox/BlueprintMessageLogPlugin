// Copyright 2022, Aquanox.

#include "BlueprintMessage.h"

#include "Kismet/KismetSystemLibrary.h"

UBlueprintMessage* UBlueprintMessage::CreateBlueprintMessage(const UObject* WorldContextObject, FName Category, EBlueprintMessageSeverity Severity, FText Message)
{
	UBlueprintMessage* Object = NewObject<UBlueprintMessage>(GetTransientPackage());
	Object->WorldContext = const_cast<UObject*>(WorldContextObject);
	Object->Category = Category.IsNone() ? TEXT("BlueprintLog") : Category;
	Object->Severity = Severity;
	Object->InitialMessage = Message;
	return Object;
}

void UBlueprintMessage::AddToken(const FBlueprintMessageToken& Token, FName Slot)
{
	// Simply add a new token
	if (Slot.IsNone())
	{
		Tokens.Add(Token);
		return;
	}

	// Find slot and replace it with new token
	if (FBlueprintMessageToken* SlotPtr = Tokens.FindByKey(Slot))
	{
		SlotPtr->Data = Token.Data;
	}
	else
	{
		FFrame::KismetExecutionMessage(
			*FString::Printf(TEXT("InitialMessage %s does not have slot with name %s."), *GetName(), *Slot.ToString()),
			ELogVerbosity::Warning
		);
	}
}

void UBlueprintMessage::AddTokens(const TArray<FBlueprintMessageToken>& InTokens)
{
	for (const FBlueprintMessageToken& Token : InTokens)
	{
		AddToken(Token);
	}
}

void UBlueprintMessage::AddSlot(FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateSlotToken(Slot));
}

void UBlueprintMessage::RemoveSlot(FName Name)
{
	for(auto It = Tokens.CreateIterator(); It; ++It)
	{
		if (It->Name == Name)
		{
			It.RemoveCurrent();
		}
	}
}

void UBlueprintMessage::ClearTokens()
{
	Tokens.Empty();
}

void UBlueprintMessage::Show()
{
	FMessageLog(Category).AddMessage(BuildMessage());
}

void UBlueprintMessage::ShowAndPrint(bool bPrintToScreen, bool bPrintToLog, FLinearColor TextColor, float Duration, const FName Key)
{
	auto MessagePtr = BuildMessage();

	FMessageLog(Category).AddMessage(MessagePtr);

	FString LongMessage;
	LongMessage.Append(Category.ToString());
	LongMessage.Append(TEXT(": "));
	LongMessage.Append(MessagePtr->ToText().ToString());

	UKismetSystemLibrary::PrintText(WorldContext.Get(), FText::FromString(LongMessage), bPrintToScreen, bPrintToLog, TextColor, Duration, Key);
}

TSharedRef<FTokenizedMessage> UBlueprintMessage::BuildMessage() const
{
	auto MessagePtr = FTokenizedMessage::Create(static_cast<EMessageSeverity::Type>(Severity), InitialMessage);

	for (const FBlueprintMessageToken& Token : Tokens)
	{
		// dump all messages except empty slots
		if (Token.Data.IsValid())
		{
			MessagePtr->AddToken(Token.Data.ToSharedRef());
		}
		else
		{
			MessagePtr->AddToken(FTextToken::Create(FText::FromString(FString::Printf(TEXT("{%s}"), *Token.Name.ToString()))));
		}
	}

	return MessagePtr;
}


void UBlueprintMessage::AddTextToken(FText Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateTextToken(Value), Slot);
}

void UBlueprintMessage::AddStringToken(FString Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateStringToken(Value), Slot);
}

void UBlueprintMessage::AddNameToken(FName Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateNameToken(Value), Slot);
}

void UBlueprintMessage::AddDynamicTextToken(FGetMessageDynamicText Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateDynamicText(Value), Slot);
}

void UBlueprintMessage::AddURLToken(FString Value, FText DisplayText, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateUrlToken(Value, DisplayText), Slot);
}

void UBlueprintMessage::AddSeverityToken(EBlueprintMessageSeverity Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateSeverityToken(Value), Slot);
}

void UBlueprintMessage::AddObjectToken(UObject* Value, FText Label, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateObjectToken(Value, Label), Slot);
}

void UBlueprintMessage::AddAssetToken(UObject* Value, FText Message, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateAssetToken(Value, Message), Slot);
}

void UBlueprintMessage::AddAssetPathToken(FSoftObjectPath Value, FText Message, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateAssetPathToken(Value, Message), Slot);
}

void UBlueprintMessage::AddImageToken(FName Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateImageToken(Value), Slot);
}

void UBlueprintMessage::AddActorToken(AActor* Value, FText Message, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateActorToken(Value, Message), Slot);
}

void UBlueprintMessage::AddTutorialToken(FString TutorialAssetName, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateTutorialToken(TutorialAssetName), Slot);
}

void UBlueprintMessage::AddDocumentationToken(FString DocumentationLink, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::CreateDocumentationToken(DocumentationLink), Slot);
}
