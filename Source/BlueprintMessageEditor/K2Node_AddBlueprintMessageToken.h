// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
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
};
