// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CreateBlueprintMessage.generated.h"

/**
 * A universal CreateBlueprintMessage with expandable AddToken pins
 */
UCLASS(MinimalAPI)
class UK2Node_CreateBlueprintMessage : public UK2Node_CallFunction, public IK2Node_AddPinInterface
{
	GENERATED_BODY()
public:
	UK2Node_CreateBlueprintMessage();

	virtual void AllocateDefaultPins() override;
	virtual bool IsNodePure() const override { return false; }

	virtual void AddInputPin() override;
	virtual void RemoveInputPin(UEdGraphPin* Pin) override;

	TArray<UEdGraphPin*> GetDynamicPins() const;
	FName GetPinName(int32 PinIndex) const;
	FName GetArrayPinName(int32 PinIndex) const;
	bool IsDynamicInputPin(const UEdGraphPin* Pin) const;
	void SyncPinNames();

	void InteractiveAddInputPin();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

protected:
	/** The number of input pins to generate for this node */
	UPROPERTY()
	int32 NumInputs;
};
