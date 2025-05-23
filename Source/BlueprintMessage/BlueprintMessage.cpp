﻿// Copyright 2022, Aquanox.

#include "BlueprintMessage.h"

#include "BlueprintMessageSettings.h"
#include "BlueprintMessageTokenFactory.h"
#include "UObject/Package.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/MessageLog.h"

UBlueprintMessage* UBlueprintMessage::CreateMessageImpl()
{
	const UBlueprintMessageSettings* Settings = UBlueprintMessageSettings::Get();
	
	UBlueprintMessage* Object = NewObject<UBlueprintMessage>(GetTransientPackage(), UBlueprintMessage::StaticClass(), NAME_None, RF_Transient|RF_DuplicateTransient);
	Object->Category = Settings->GetDefaultCategory();
	Object->bSuppressLoggingToOutputLog = Settings->bDefaultSuppressLoggingToOutputLog;
	Object->bAutoDestroy = Settings->bDefaultAutoDestroy;

	return Object;
}

UBlueprintMessage* UBlueprintMessage::CreateBlueprintMessage(FName LogCategory, EBlueprintMessageSeverity Severity)
{
	UBlueprintMessage* Object = CreateMessageImpl();
	Object->Category = LogCategory;
	Object->Severity = Severity;
	return Object;
}

UBlueprintMessage* UBlueprintMessage::CreateSimpleBlueprintMessage(FName LogCategory, EBlueprintMessageSeverity Severity, FText Message, bool bCallShow)
{
	UBlueprintMessage* Object = CreateMessageImpl();
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
	const FName ActualCategory = Category.IsNone() ? UBlueprintMessageSettings::Get()->GetDefaultCategory() : Category;
	FMessageLog(ActualCategory).Open(
		static_cast<EMessageSeverity::Type>(Severity),
		bForce
	);
}

void UBlueprintMessage::MessageLogNotify(FText Message, FName Category, EBlueprintMessageSeverity Severity, bool bForce)
{
	const FName ActualCategory = Category.IsNone() ? UBlueprintMessageSettings::Get()->GetDefaultCategory() : Category;
	FMessageLog(ActualCategory).Notify(
		Message,
		static_cast<EMessageSeverity::Type>(Severity),
		bForce
	);
}

UBlueprintMessage::UBlueprintMessage()
{
	UE_LOG(LogBlueprintMessage, Verbose, TEXT("Construct UBlueprintMessage at %p"), this);
}

UBlueprintMessage::~UBlueprintMessage()
{
	UE_LOG(LogBlueprintMessage, Verbose, TEXT("Destroy UBlueprintMessage at %p"), this);
}

TArray<FName> UBlueprintMessage::GetAvailableCategories()
{
	TArray<FName> Result;
	UBlueprintMessageSettings::Get()->GetAvailableCategories(Result);
	return Result;
}

UBlueprintMessage* UBlueprintMessage::Duplicate()
{
	UBlueprintMessage* Object = CreateMessageImpl();
	Object->Category = Category;
	Object->Severity = Severity;
	Object->InitialMessage = InitialMessage;
	Object->Tokens = Tokens;
	Object->bSuppressLoggingToOutputLog = bSuppressLoggingToOutputLog;
	Object->bAutoDestroy = bAutoDestroy;
	return Object;
}

void UBlueprintMessage::Destroy()
{
	Tokens.Empty();
	MarkAsGarbage();
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
	{
		FTagToMessage TagToMessage = BuildMessage();
		ShowImpl(TagToMessage.Key, TagToMessage.Value);
	}
#endif
	
	if (bAutoDestroy)
	{
		Destroy();
	}
}

void UBlueprintMessage::ShowAndPrint(bool bPrintToScreen, bool bPrintToLog, FLinearColor TextColor, float Duration, const FName Key)
{
#if WITH_EDITOR
	{
		FTagToMessage TagToMessage = BuildMessage();

		ShowImpl(TagToMessage.Key, TagToMessage.Value);

		FStringBuilderBase LongMessage;
		LongMessage.Append(TagToMessage.Key.ToString());
		LongMessage.Append(TEXT(": "));
		LongMessage.Append(TagToMessage.Value->ToText().ToString());

		UKismetSystemLibrary::PrintText(nullptr, FText::FromString(LongMessage.ToString()), bPrintToScreen, bPrintToLog, TextColor, Duration, Key);
	}
#endif
	
	if (bAutoDestroy)
	{
		Destroy();
	}
}

void UBlueprintMessage::ShowImpl(const FName& InCategory, const TSharedRef<FTokenizedMessage>& InMessage) const
{
	UE_LOG(LogBlueprintMessage, Verbose, TEXT("Show UBlueprintMessage at %p to target %s with %d tokens"), this, *InCategory.ToString(), Tokens.Num());
	
#if WITH_EDITOR
	{
		FMessageLog Log(InCategory);
		Log.SuppressLoggingToOutputLog(bSuppressLoggingToOutputLog);
		Log.AddMessage(InMessage);
		Log.Flush();
		// ~FMessageLog()
	}
#endif
}

UBlueprintMessage::FTagToMessage UBlueprintMessage::BuildMessage() const
{
	FName MessageCat = Category;
	if (MessageCat.IsNone())
	{
		MessageCat = UBlueprintMessageSettings::Get()->GetDefaultCategory();
	}
	
	TSharedRef<FTokenizedMessage> MessagePtr = FTokenizedMessage::Create(static_cast<EMessageSeverity::Type>(Severity), InitialMessage);

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

	return MakeTuple(MessageCat,  MessagePtr);
}
