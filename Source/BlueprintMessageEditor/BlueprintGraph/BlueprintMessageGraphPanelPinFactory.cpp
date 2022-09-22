// Copyright 2022, Aquanox.
#include "BlueprintGraph/BlueprintMessageGraphPanelPinFactory.h"

#include "BlueprintGraph/SmartGraphPanelPinFactory.h"
#include "BlueprintGraph/SmartGraphPanelPinFactoryMatchers.h"
#include "K2Node_CallFunction.h"
#include "PropertyPathHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SGraphPinNameList.h"
#include "Slate/SGraphPinNameCombobox.h"

TSharedPtr<FGraphPanelPinFactory> UBlueprintMessageGraphPanelPinFactory::CreateFactory() const
{
	auto Factory = MakeShared<FSmartGraphPanelPinFactory>();
	Factory->CreateHandler(TEXT("GetOptionsFromMeta"))
		.AddMatcher<FPinFactoryMatcher_Schema>(UEdGraphSchema_K2::StaticClass())
		.AddMatcher<FPinFactoryMatcher_PinCategory>(UEdGraphSchema_K2::PC_Name)
		.AddMatcher<FPinFactoryMatcher_Node>(UK2Node_CallFunction::StaticClass())
		.AddMatcher<FPinFactoryMatcher_PinHasMetadata>(TEXT("GetOptions"))
		.Handle(FGraphPinHandlerDelegate::CreateUObject(this, &ThisClass::CreateGetOptionsPin));

	return Factory;
}

TSharedPtr<SGraphPin> UBlueprintMessageGraphPanelPinFactory::CreateGetOptionsPin(UEdGraphPin* InPin) const
{
	static const FName MD_GetOptions(TEXT("GetOptions"));

	ensure(InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name);

	UObject* const Outer = Cast<UK2Node_CallFunction>(InPin->GetOuter());
	ensure(Outer);

	FString FunctionName;
	TArray<UObject*> TargetObjects;

	if (UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(Outer))
	{
		UFunction* CallTargetFunction = CallFunctionNode->GetTargetFunction();
		FString PinMetaData = CallFunctionNode->GetPinMetaData(InPin->GetFName(), MD_GetOptions);
		if (!PinMetaData.IsEmpty() && CallTargetFunction)
		{
			FunctionName = PinMetaData;
			TargetObjects.Add(CallTargetFunction->GetOuterUClass()->GetDefaultObject());
		}
	}

	TArray<TSharedPtr<FName>> Options;
	if (!BuildSelectableOptions(TargetObjects, FunctionName, Options))
	{
		return nullptr;
	}

	return SNew(SGraphPinNameCombobox, InPin, Options);
}

bool UBlueprintMessageGraphPanelPinFactory::BuildSelectableOptions(TArray<UObject*>& SourceObjects, FString SourceFunctionName, TArray<TSharedPtr<FName>>& OutOptions) const
{
	if (SourceFunctionName.IsEmpty())
	{
		return false;
	}

	// Check for external function references
	if (SourceFunctionName.Contains(TEXT(".")))
	{
		SourceObjects.Empty();
		UFunction* GetOptionsFunction = FindObject<UFunction>(nullptr, *SourceFunctionName, true);

		if (ensureMsgf(GetOptionsFunction && GetOptionsFunction->HasAnyFunctionFlags(EFunctionFlags::FUNC_Static), TEXT("Invalid GetOptions: %s"), *SourceFunctionName))
		{
			UObject* GetOptionsCDO = GetOptionsFunction->GetOuterUClass()->GetDefaultObject();
			SourceFunctionName = GetOptionsFunction->GetName();
			SourceObjects.Add(GetOptionsCDO);
		}
	}

	if (SourceObjects.Num() > 0)
	{
		TArray<FString> OptionIntersection;
		TSet<FString> OptionIntersectionSet;

		for (UObject* Target : SourceObjects)
		{
			TArray<FString> StringOptions;
			{
				FEditorScriptExecutionGuard ScriptExecutionGuard;

				FCachedPropertyPath Path(SourceFunctionName);
				if (!PropertyPathHelpers::GetPropertyValue(Target, Path, StringOptions))
				{
					TArray<FName> NameOptions;
					if (PropertyPathHelpers::GetPropertyValue(Target, Path, NameOptions))
					{
						Algo::Transform(NameOptions, StringOptions, [](const FName& InName)
						{
							return InName.ToString();
						});
					}
				}
			}

			// If this is the first time there won't be any options.
			if (OptionIntersection.Num() == 0)
			{
				OptionIntersection = StringOptions;
				OptionIntersectionSet = TSet<FString>(StringOptions);
			}
			else
			{
				TSet<FString> StringOptionsSet(StringOptions);
				OptionIntersectionSet = StringOptionsSet.Intersect(OptionIntersectionSet);
				OptionIntersection.RemoveAll([&OptionIntersectionSet](const FString& Option)
				{
					return !OptionIntersectionSet.Contains(Option);
				});
			}

			// If we're out of possible intersected options, we can stop.
			if (OptionIntersection.Num() == 0)
			{
				break;
			}
		}

		Algo::Transform(OptionIntersection, OutOptions, [](const FString& InString)
		{
			return MakeShared<FName>(*InString);
		});
	}

	return OutOptions.Num() > 0;
}
