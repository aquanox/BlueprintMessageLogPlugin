// Copyright 2022, Aquanox.
#pragma once

#include "SGraphPin.h"

class SGraphPinNameCombobox : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPinNameCombobox) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, const TArray<TSharedPtr<FName>>& InNameList);

protected:

	/**
	 *	Function to create class specific widget.
	 *
	 *	@return Reference to the newly created widget object
	 */
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;

private:

	FString OnGetText() const;
	FText OnGetFriendlyName(int32 EnumIndex) const;

	void GenerateComboBoxIndexes(TArray<TSharedPtr<int32>>& OutComboBoxIndexes);

	/**
	 *	Function to set the newly selected index
	 *
	 * @param NewSelection The newly selected item in the combo box
	 * @param SelectInfo Provides context on how the selection changed
	 */
	void ComboBoxSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo);

	TSharedPtr<class SPinComboBox> ComboBox;

	/** The actual list of FName values to choose from */
	TArray<TSharedPtr<FName>> NameList;
};

