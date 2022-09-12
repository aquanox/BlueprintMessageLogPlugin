// Copyright 2022, Aquanox.
#include "BlueprintGraph/SGraphPinNameCombobox.h"

#include "SGraphPinComboBox.h"

void SGraphPinNameCombobox::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, const TArray<TSharedPtr<FName>>& InNameList)
{
	NameList = InNameList;
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SGraphPinNameCombobox::GetDefaultValueWidget()
{
	TArray<TSharedPtr<int32>> Indexes;
	GenerateComboBoxIndexes(Indexes);

	//Create widget
	return SAssignNew(ComboBox, SPinComboBox)
		.ComboItemList(Indexes)
		.VisibleText(this, &SGraphPinNameCombobox::OnGetText)
		.OnGetDisplayName(this, &SGraphPinNameCombobox::OnGetFriendlyName)
		.OnGetTooltip(this, &SGraphPinNameCombobox::OnGetFriendlyName)
		.OnSelectionChanged(this, &SGraphPinNameCombobox::ComboBoxSelectionChanged)
		.IsEnabled(this, &SGraphPinNameCombobox::GetDefaultValueIsEditable)
		.Visibility(this, &SGraphPinNameCombobox::GetDefaultValueVisibility);
}

void SGraphPinNameCombobox::GenerateComboBoxIndexes(TArray<TSharedPtr<int32>>& OutComboBoxIndexes)
{
	for (int32 EnumIndex = 0; EnumIndex < NameList.Num(); ++EnumIndex)
	{
		OutComboBoxIndexes.Add(MakeShared<int32>(EnumIndex));
	}
}

void SGraphPinNameCombobox::ComboBoxSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type /*SelectInfo*/)
{
	FName Name = NewSelection.IsValid() ? *NameList[*NewSelection] : NAME_None;
	if (auto Schema = (GraphPinObj ? GraphPinObj->GetSchema() : NULL))
	{
		FString NameAsString = Name.ToString();
		if (GraphPinObj->GetDefaultAsString() != NameAsString)
		{
			const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeNameComboboxPinValue", "Change Name Combobox Pin Value"));
			GraphPinObj->Modify();

			Schema->TrySetDefaultValue(*GraphPinObj, NameAsString);
		}
	}
}

FString SGraphPinNameCombobox::OnGetText() const
{
	const FString SelectedString = GraphPinObj->GetDefaultAsString();

	TSharedPtr<FName> CurrentlySelectedName;
	if (GraphPinObj)
	{
		// Preserve previous selection, or set to first in list
		FName PreviousSelection = FName(*SelectedString);
		for (TSharedPtr<FName> ListNamePtr : NameList)
		{
			if (PreviousSelection == *ListNamePtr.Get())
			{
				CurrentlySelectedName = ListNamePtr;
				break;
			}
		}
	}

	return FName::NameToDisplayString(CurrentlySelectedName->ToString(), false);
}

FText SGraphPinNameCombobox::OnGetFriendlyName(int32 EnumIndex) const
{
	if (!NameList.IsValidIndex(EnumIndex))
	{
		return INVTEXT("INVALID");
	}

	return FText::FromString(
		FName::NameToDisplayString(NameList[EnumIndex]->ToString(), false)
	);
}
