// Copyright 2022, Aquanox.

#include "BlueprintMessage.h"

#include "BlueprintMessageSettings.h"
#include "BlueprintMessageTokenFactory.h"
#include "UObject/Package.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/MessageLog.h"

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

void UBlueprintMessage::MessageLogOpen(FName Category, EBlueprintMessageSeverity Severity, bool bForce)
{
	const FName ActualCategory = Category.IsNone() ? UBlueprintMessageSettings::DefaultCategory : Category;
	FMessageLog(ActualCategory).Open(
		static_cast<EMessageSeverity::Type>(Severity),
		bForce
	);
}

void UBlueprintMessage::MessageLogNotify(FText Message, FName Category, EBlueprintMessageSeverity Severity, bool bForce)
{
	const FName ActualCategory = Category.IsNone() ? UBlueprintMessageSettings::DefaultCategory : Category;
	FMessageLog(ActualCategory).Notify(
		Message,
		static_cast<EMessageSeverity::Type>(Severity),
		bForce
	);
}

UBlueprintMessage::UBlueprintMessage()
{
}

TArray<FName> UBlueprintMessage::GetAvailableCategories()
{
	TArray<FName> Result;
	UBlueprintMessageSettings::Get()->GetAvailableCategories(Result);
	return Result;
}

UBlueprintMessage* UBlueprintMessage::Duplicate()
{
	UBlueprintMessage* Object = CreateSimpleBlueprintMessage(Category, Severity, InitialMessage);
	Object->Tokens = Tokens;
	return Object;
}

UBlueprintMessage* UBlueprintMessage::AddToken(const FBlueprintMessageToken& Token, FName Slot)
{
	// Token with nothing in it
	if (Token.Name.IsNone() && !Token.Instance.IsValid())
	{
		return this;
	}

	// Simply add a new token
	if (Slot.IsNone())
	{
		Tokens.Add(Token);
		return this;
	}

	// Find slot and replace it with new token
	if (FBlueprintMessageToken* SlotPtr = Tokens.FindByKey(Slot))
	{
		SlotPtr->Instance = Token.Instance;
	}
	else
	{ // Add new token with slot identifier
		Tokens.Add_GetRef(Token).Name = Slot;
	}

	return this;
}

UBlueprintMessage* UBlueprintMessage::AddTokens(const TArray<FBlueprintMessageToken>& InTokens)
{
	for (const FBlueprintMessageToken& Token : InTokens)
	{
		AddToken(Token);
	}
	return this;
}

UBlueprintMessage* UBlueprintMessage::AddNamedSlot(FName Slot)
{
	AddToken(FBlueprintMessageToken(Slot));
	return this;
}

UBlueprintMessage* UBlueprintMessage::RemoveNamedSlot(FName Name)
{
	for(auto It = Tokens.CreateIterator(); It; ++It)
	{
		if (It->Name == Name)
		{
			It.RemoveCurrent();
		}
	}
	return this;
}

UBlueprintMessage* UBlueprintMessage::SetSeverity(EBlueprintMessageSeverity NewSeverity)
{
	Severity = NewSeverity;
	return this;
}

UBlueprintMessage* UBlueprintMessage::ClearTokens()
{
	Tokens.Empty();
	return this;
}

void UBlueprintMessage::Show()
{
#if WITH_EDITOR
	const FName ActualCategory = Category.IsNone() ? TEXT("BlueprintLog") : Category;
	ShowImpl(ActualCategory, BuildMessage());
#endif
}

void UBlueprintMessage::ShowAndPrint(bool bPrintToScreen, bool bPrintToLog, FLinearColor TextColor, float Duration, const FName Key)
{
#if WITH_EDITOR
	const FName ActualCategory = Category.IsNone() ? TEXT("BlueprintLog") : Category;
	TSharedRef<FTokenizedMessage> MessagePtr = BuildMessage();

	ShowImpl(ActualCategory, MessagePtr);

	FString LongMessage;
	LongMessage.Append(ActualCategory.ToString());
	LongMessage.Append(TEXT(": "));
	LongMessage.Append(MessagePtr->ToText().ToString());

	UKismetSystemLibrary::PrintText(nullptr, FText::FromString(LongMessage), bPrintToScreen, bPrintToLog, TextColor, Duration, Key);
#endif
}

void UBlueprintMessage::ShowImpl(const FName& InCategory, const TSharedRef<FTokenizedMessage>& InMessage) const
{
#if WITH_EDITOR
	{
		FMessageLog Log(InCategory);
		Log.SuppressLoggingToOutputLog(bSuppressLoggingToOutputLog);
		Log.AddMessage(InMessage);
		// ~FMessageLog() -> Flush()
	}
#endif
}


TSharedRef<FTokenizedMessage> UBlueprintMessage::BuildMessage() const
{
	auto MessagePtr = FTokenizedMessage::Create(static_cast<EMessageSeverity::Type>(Severity), InitialMessage);

	for (const FBlueprintMessageToken& Token : Tokens)
	{
		// dump all messages except empty slots
		if (Token.Instance.IsValid())
		{
			MessagePtr->AddToken(Token.Instance.ToSharedRef());
		}
		else
		{
			MessagePtr->AddToken(FTextToken::Create(FText::FromString(FString::Printf(TEXT("{%s}"), *Token.Name.ToString()))));
		}
	}

	return MessagePtr;
}
