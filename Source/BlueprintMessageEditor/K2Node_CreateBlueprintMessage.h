// Copyright 2022, Aquanox.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CreateBlueprintMessage.generated.h"

/**
 * A universal CreateBlueprintMessage with expandable AddToken pins
 */
UCLASS(MinimalAPI)
class UK2Node_CreateBlueprintMessage : public UK2Node_CallFunction
{
	GENERATED_BODY()
};
