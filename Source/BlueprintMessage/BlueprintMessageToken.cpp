// Copyright 2022, Aquanox.

#include "BlueprintMessageToken.h"
#include "UObject/Package.h"
#include "UObject/UObjectIterator.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/EngineVersionComparison.h"
#include "Logging/MessageLog.h"
#include "Logging/TokenizedMessage.h"

const FName FMessageTokenFactoryRegistration::MD_TokenFactoryFunction(TEXT("TokenFactoryFunction"));
const FName FMessageTokenFactoryRegistration::MD_TokenFactoryName(TEXT("TokenFactoryName"));

FBlueprintMessageToken::FBlueprintMessageToken(const FName& InSlot): Name(InSlot)
{
}

FBlueprintMessageToken::FBlueprintMessageToken(TSharedRef<IMessageToken>&& InToken)
	: Instance(MoveTemp(InToken))
{
}

FBlueprintMessageToken& FBlueprintMessageToken::OnMessageTokenActivated(FOnBlueprintMessageTokenActivated&& Delegate)
{
	if (ensure(Instance.IsValid()))
	{
		Instance->OnMessageTokenActivated(Delegate);
	}
	return *this;
}

FMessageTokenFactoryRegistration::FMessageTokenFactoryRegistration(UFunction* Function)
	: FactoryFunction(Function), FactoryClass(Function->GetOuterUClass()), FunctionName(Function->GetFName())
{
}

bool FMessageTokenFactoryRegistration::IsValid() const
{
	return FactoryFunction.IsValid() && FactoryClass.IsValid() && !FunctionName.IsNone();
}

bool FMessageTokenFactoryRegistration::IsTokenFactoryFunction(UFunction* Function)
{
#if WITH_EDITOR
	if (!::IsValid(Function))
		return false;

	if (!Function->HasMetaData(MD_TokenFactoryFunction))
		return false;

	if (!Function->HasAllFunctionFlags(FUNC_Static | FUNC_Native))
		return false;

	FStructProperty* RetVal = CastField<FStructProperty>(Function->GetReturnProperty());
	if (!RetVal || RetVal->Struct != FBlueprintMessageToken::StaticStruct())
		return false;

	return true;
#else
	return false;
#endif
}

void FMessageTokenFactoryRegistration::GetRegisteredFactories(TArray<FMessageTokenFactoryRegistration>& OutArray)
{
	for (TObjectIterator<UFunction> It; It; ++It)
	{
		UFunction* const Function = *It;
		if (FMessageTokenFactoryRegistration::IsTokenFactoryFunction(Function))
		{
			OutArray.Add(FMessageTokenFactoryRegistration(Function));
		}
	}
}
