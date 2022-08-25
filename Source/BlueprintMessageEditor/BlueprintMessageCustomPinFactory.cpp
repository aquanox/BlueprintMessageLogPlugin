﻿// Copyright 2022, Aquanox.
#include "BlueprintMessageCustomPinFactory.h"

#include "K2Node_CallFunction.h"
#include "PropertyPathHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SGraphPinNameList.h"
#include "SGraphPinNameCombobox.h"

TSharedPtr<SGraphPin> FBlueprintMessageCustomPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	static const FName MD_GetOptions(TEXT("GetOptions"));

	if (InPin->GetSchema()->IsA(UEdGraphSchema_K2::StaticClass())
		&& InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
	{
		TArray<UObject*> OutObjects;

		UObject* Outer = InPin->GetOuter();

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
					OutObjects.Add(OuterUClass->GetDefaultObject());
					// register current blueprint as secondary outer?
				}
			}
		}

		TArray<TSharedPtr<FName>> OutOptions;

		// @from prop details
		if (!GetOptionsFunctionName.IsEmpty())
		{
			// Check for external function references
			if (GetOptionsFunctionName.Contains(TEXT(".")))
			{
				OutObjects.Empty();
				UFunction* GetOptionsFunction = FindObject<UFunction>(nullptr, *GetOptionsFunctionName, true);

				if (ensureMsgf(GetOptionsFunction && GetOptionsFunction->HasAnyFunctionFlags(EFunctionFlags::FUNC_Static), TEXT("Invalid GetOptions: %s"), *GetOptionsFunctionName))
				{
					UObject* GetOptionsCDO = GetOptionsFunction->GetOuterUClass()->GetDefaultObject();
					GetOptionsFunction->GetName(GetOptionsFunctionName);
					OutObjects.Add(GetOptionsCDO);
				}
			}

			if (OutObjects.Num() > 0)
			{
				TArray<FString> OptionIntersection;
				TSet<FString> OptionIntersectionSet;

				for (UObject* Target : OutObjects)
				{
					TArray<FString> StringOptions;
					{
						FEditorScriptExecutionGuard ScriptExecutionGuard;

						FCachedPropertyPath Path(GetOptionsFunctionName);
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

		if (OutOptions.Num())
		{
		//	return SNew(SGraphPinNameList, InPin, OutOptions);
			return SNew(SGraphPinNameCombobox, InPin, OutOptions);
		}
	}

	return nullptr;
}