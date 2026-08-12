// Copyright 2022, Aquanox.


#include "BlueprintMessageLibrary.h"
#include "BlueprintMessageSettings.h"
#include "Logging/MessageLog.h"

#if WITH_EDITOR
#include "Modules/ModuleManager.h"
#include "MessageLogModule.h"
#include "IMessageLogListing.h"
#endif

TArray<FName> UBlueprintMessageLibrary::GetAvailableCategories()
{
	return UBlueprintMessageSettings::Get()->GetGraphSelectableCategories();
}

void UBlueprintMessageLibrary::MessageLogOpen(FName Category, EBlueprintMessageSeverity Severity, bool bForce)
{
#if WITH_EDITOR
	const FName ActualCategory = Category.IsNone() ? UBlueprintMessageSettings::Get()->GetDefaultCategory() : Category;
	FMessageLog(ActualCategory).Open(
		static_cast<EMessageSeverity::Type>(Severity),
		bForce
	);
#endif
}

void UBlueprintMessageLibrary::MessageLogNotify(FText Message, FName Category, EBlueprintMessageSeverity Severity, bool bForce)
{
#if WITH_EDITOR
	const FName ActualCategory = Category.IsNone() ? UBlueprintMessageSettings::Get()->GetDefaultCategory() : Category;
	FMessageLog(ActualCategory).Notify(
		Message,
		static_cast<EMessageSeverity::Type>(Severity),
		bForce
	);
#endif
}

void UBlueprintMessageLibrary::MessageLogClear(FName Category)
{
#if WITH_EDITOR
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	if (MessageLogModule.IsRegisteredLogListing(Category))
	{
		MessageLogModule.GetLogListing(Category)->ClearMessages();
	}
#endif
}

FString UBlueprintMessageLibrary::MessageLogCopyAllMessages(FName Category)
{
#if WITH_EDITOR
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	if (MessageLogModule.IsRegisteredLogListing(Category))
	{
		MessageLogModule.GetLogListing(Category)->GetAllMessagesAsString();
	}
#endif
	return FString();
}
