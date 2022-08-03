// Copyright 2022, Aquanox.

#include "BlueprintMessageToken.h"
#include "BlueprintMessageModule.h"
#include "Logging/TokenizedMessage.h"

const FName FMessageTokenFactoryRegistration::MD_TokenFactoryFunction(TEXT("TokenFactoryFunction"));
const FName FMessageTokenFactoryRegistration::MD_TokenFactoryName(TEXT("TokenFactoryName"));

FBlueprintMessageToken::FBlueprintMessageToken(FName InSlot): Name(InSlot)
{
}

FBlueprintMessageToken::FBlueprintMessageToken(TSharedRef<IMessageToken> InToken): Data(InToken)
{
}

FMessageTokenFactoryRegistration::FMessageTokenFactoryRegistration(UFunction* Function)
	: FactoryClass(Function->GetOuterUClass()), FunctionName(Function->GetFName())
{
}

bool FMessageTokenFactoryRegistration::IsTokenFactoryFunction(UFunction& Function)
{
	if (!Function.HasMetaData(MD_TokenFactoryFunction))
		return false;

	if (!Function.HasAllFunctionFlags(FUNC_Static | FUNC_Native | FUNC_BlueprintPure))
		return false;

	FStructProperty* RetVal = CastField<FStructProperty>(Function.GetReturnProperty());
	if (!RetVal || RetVal->Struct != FBlueprintMessageToken::StaticStruct())
		return false;

	return true;
}

void FMessageTokenFactoryRegistration::GetRegisteredFactories(TArray<FMessageTokenFactoryRegistration>& OutArray)
{
	for (TObjectIterator<UFunction> It; It; ++It)
	{
		UFunction* const Function = *It;

		const UClass* FuncOuter = Cast<UClass>(Function->GetOuter());

		if (FuncOuter && FMessageTokenFactoryRegistration::IsTokenFactoryFunction(*Function))
		{
			OutArray.Add(FMessageTokenFactoryRegistration(Function));
		}
	}
}
