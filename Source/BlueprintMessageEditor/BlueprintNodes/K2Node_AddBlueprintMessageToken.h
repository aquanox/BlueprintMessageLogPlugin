// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_AddBlueprintMessageToken.generated.h"

/**
 * Dynamic Create Token + Add Token combo
 *
 * Generate pin for Message
 * Generate pin for Create*Token
 * Generate output pin for Message
 */
UCLASS(MinimalAPI)
class UK2Node_AddBlueprintMessageToken : public UK2Node_CallFunction
{
	GENERATED_BODY()
public:
	UK2Node_AddBlueprintMessageToken();

	virtual void AllocateDefaultPins() override;
	virtual bool IsNodePure() const override { return false; }

	UFunction* GetFactoryFunction() const;

	FText GetTokenTitle() const;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void AddSearchMetaDataInfo(TArray<struct FSearchTagDataPair>& OutTaggedMetaData) const override;
	virtual FText GetFunctionContextString() const override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;

	virtual FString GetPinMetaData(FName InPinName, FName InKey) override;

protected:
	UPROPERTY()
	FMemberReference FactoryReference;
};
