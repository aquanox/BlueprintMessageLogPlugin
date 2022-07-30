// Copyright 2022, Aquanox.

#include "BlueprintMessage.h"

#include "Kismet/KismetSystemLibrary.h"

UBlueprintMessage* UBlueprintMessage::CreateBlueprintMessage(FName Category, EBlueprintMessageSeverity Severity)
{
	return CreateSimpleBlueprintMessage(Category, Severity, FText::GetEmpty(), false);
}

UBlueprintMessage* UBlueprintMessage::CreateSimpleBlueprintMessage(FName LogCategory, EBlueprintMessageSeverity Severity, FText Message, bool bCallShow)
{
	UBlueprintMessage* Object = NewObject<UBlueprintMessage>(GetTransientPackage(), UBlueprintMessage::StaticClass(), NAME_None, RF_Transient|RF_DuplicateTransient);
	Object->Category = LogCategory;
	Object->Severity = Severity;
	Object->InitialMessage = Message;
	if (bCallShow)
	{
		Object->Show();
	}
	return Object;
}

UBlueprintMessage::UBlueprintMessage()
{
}

UBlueprintMessage* UBlueprintMessage::Duplicate()
{
	UBlueprintMessage* Object = CreateSimpleBlueprintMessage(Category, Severity, InitialMessage);
	Object->Tokens = Tokens;
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
	{ // Add new token with slot identifier
		Tokens.Add_GetRef(Token).Name = Slot;
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
	AddToken(UBlueprintMessageTokenFactory::MakeSlotToken(Slot));
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
#if WITH_EDITOR
	FName ActualCategory = Category.IsNone() ? TEXT("BlueprintLog") : Category;
	FMessageLog(ActualCategory).AddMessage(BuildMessage());
#endif
}

void UBlueprintMessage::ShowAndPrint(bool bPrintToScreen, bool bPrintToLog, FLinearColor TextColor, float Duration, const FName Key)
{
#if WITH_EDITOR
	auto MessagePtr = BuildMessage();

	FName ActualCategory = Category.IsNone() ? TEXT("BlueprintLog") : Category;
	FMessageLog(ActualCategory).AddMessage(MessagePtr);

	FString LongMessage;
	LongMessage.Append(ActualCategory.ToString());
	LongMessage.Append(TEXT(": "));
	LongMessage.Append(MessagePtr->ToText().ToString());

	UKismetSystemLibrary::PrintText(nullptr, FText::FromString(LongMessage), bPrintToScreen, bPrintToLog, TextColor, Duration, Key);
#endif
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
	AddToken(UBlueprintMessageTokenFactory::MakeTextToken(Value), Slot);
}

void UBlueprintMessage::AddStringToken(FString Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeStringToken(Value), Slot);
}

void UBlueprintMessage::AddNameToken(FName Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeNameToken(Value), Slot);
}

void UBlueprintMessage::AddURLToken(FString Value, FText Label, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeUrlToken(Value, Label), Slot);
}

void UBlueprintMessage::AddSeverityToken(EBlueprintMessageSeverity Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeSeverityToken(Value), Slot);
}

void UBlueprintMessage::AddObjectToken(UObject* Value, FText Label, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeObjectToken(Value, Label), Slot);
}

void UBlueprintMessage::AddAssetToken(UObject* Value, FText Message, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeAssetToken(Value, Message), Slot);
}

void UBlueprintMessage::AddAssetSoftPtrToken(TSoftObjectPtr<UObject> Value, FText Message, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeAssetSoftPtrToken(Value, Message), Slot);
}

void UBlueprintMessage::AddAssetPathToken(FSoftObjectPath Value, FText Message, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeAssetPathToken(Value, Message), Slot);
}

void UBlueprintMessage::AddImageToken(FName Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeImageToken(Value), Slot);
}

void UBlueprintMessage::AddActorToken(AActor* Value, FText Message, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeActorToken(Value, Message), Slot);
}

void UBlueprintMessage::AddTutorialToken(FString TutorialAssetName, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeTutorialToken(TutorialAssetName), Slot);
}

void UBlueprintMessage::AddDocumentationToken(FString DocumentationLink, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeDocumentationToken(DocumentationLink), Slot);
}

void UBlueprintMessage::AddDynamicTextToken_Delegate(FGetMessageDynamicText Value, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeDynamicTextToken_Delegate(Value), Slot);
}

void UBlueprintMessage::AddDynamicTextToken_Function(UObject* Object, FName FunctionName, FName Slot)
{
	AddToken(UBlueprintMessageTokenFactory::MakeDynamicTextToken_Function(Object, FunctionName), Slot);
}
