// Copyright 2022, Aquanox.
#include "BlueprintGraph/BlueprintMessageCustomPinFactory.h"

#include "K2Node_CallFunction.h"
#include "PropertyPathHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SGraphPinNameList.h"
#include "SGraphPinNameCombobox.h"

FBlueprintMessageCustomPinFactory::FBlueprintMessageCustomPinFactory()
{
	CreateHandler(TEXT("GetOptionsHandler"))
		.AddMatcher<FPinFactoryMatcher_Schema>(UEdGraphSchema_K2::StaticClass())
		.AddMatcher<FPinFactoryMatcher_PinCategory>(UEdGraphSchema_K2::PC_Name)
		.AddMatcher<FPinFactoryMatcher_Node>(UK2Node_CallFunction::StaticClass())
		.AddMatcher<FPinFactoryMatcher_PinMetadata>(TEXT("GetOptions"))
		.Handle(FHandlerDelegate::CreateRaw(this, &FBlueprintMessageCustomPinFactory::CreateGetOptionsPin));
}

TSharedPtr<SGraphPin> FBlueprintMessageCustomPinFactory::CreateGetOptionsPin(UEdGraphPin* InPin) const
{
	static const FName MD_GetOptions(TEXT("GetOptions"));

	ensure(InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name);

	UObject* const Outer = Cast<UK2Node_CallFunction>(InPin->GetOuter());
	ensure(Outer);

	TArray<UObject*> GetOptionTargetObjects;

	FString GetOptionsFunctionName;
	if (Outer->IsA(UK2Node_CallFunction::StaticClass()))
	{
		if (UK2Node_CallFunction* CallFunctionNode = CastChecked<UK2Node_CallFunction>(Outer))
		{
			FString PinMetaData = CallFunctionNode->GetPinMetaData(InPin->GetFName(), MD_GetOptions);
			if (!PinMetaData.IsEmpty() && CallFunctionNode->GetTargetFunction())
			{
				GetOptionsFunctionName = PinMetaData;

				// register target CDO as default outer
				UClass* OuterUClass = CallFunctionNode->GetTargetFunction()->GetOuterUClass();
				GetOptionTargetObjects.Add(OuterUClass->GetDefaultObject());
				// register current blueprint as secondary outer?
			}
		}
	}

	TArray<TSharedPtr<FName>> GetOptionsOptions;
	if (!GetOptionsFunctionName.IsEmpty())
	{
		GetOptionsToSelect(GetOptionTargetObjects, GetOptionsFunctionName, GetOptionsOptions);
	}

	if (GetOptionsOptions.Num())
	{
		return SNew(SGraphPinNameCombobox, InPin, GetOptionsOptions);
	}

	return nullptr;
}

void FBlueprintMessageCustomPinFactory::GetOptionsToSelect(TArray<UObject*>& SourceObjects, FString SourceFunctionName, TArray<TSharedPtr<FName>>& OutOptions) const
{
	check(!SourceFunctionName.IsEmpty());

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
						Algo::Transform(NameOptions, StringOptions, [](const FName& InName) { return InName.ToString(); });
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
				OptionIntersection.RemoveAll([&OptionIntersectionSet](const FString& Option) { return !OptionIntersectionSet.Contains(Option); });
			}

			// If we're out of possible intersected options, we can stop.
			if (OptionIntersection.Num() == 0)
			{
				break;
			}
		}

		Algo::Transform(OptionIntersection, OutOptions, [](const FString& InString) { return MakeShared<FName>(*InString); });
	}
}
