// Copyright 2022, Aquanox.

#include "BlueprintMessageToken.h"
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

FBlueprintMessageToken::FBlueprintMessageToken(TSharedRef<IMessageToken>&& InToken, TSharedRef<FBlueprintMessageTokenData>&& InCustomData)
	: Instance(MoveTemp(InToken)), Data(MoveTemp(InCustomData))
{
}

void FBlueprintMessageToken::OnMessageTokenActivated(FOnBlueprintMessageTokenActivated&& InDelegate)
{
	if (!Instance.IsValid())
	{
		return;
	}

	Instance->OnMessageTokenActivated(FOnMessageTokenActivated::CreateLambda([Token = this, Delegate = MoveTemp(InDelegate)](const TSharedPtr<IMessageToken>& TokenInstance)
	{
		if (Token->Instance.IsValid())
		{
			ensure(TokenInstance == Token->Instance);
			Delegate.Execute(*Token);
		}
	}));
}

FMessageTokenFactoryRegistration::FMessageTokenFactoryRegistration(UFunction* Function)
	: FactoryClass(Function->GetOuterUClass()), FunctionName(Function->GetFName())
{
}

bool FMessageTokenFactoryRegistration::IsTokenFactoryFunction(UFunction* Function)
{
	if (!IsValid(Function) || !Function->GetOuterUClass())
		return false;

	if (!Function->HasMetaData(MD_TokenFactoryFunction))
		return false;

	if (!Function->HasAllFunctionFlags(FUNC_Static | FUNC_Native | FUNC_BlueprintPure))
		return false;

	FStructProperty* RetVal = CastField<FStructProperty>(Function->GetReturnProperty());
	if (!RetVal || RetVal->Struct != FBlueprintMessageToken::StaticStruct())
		return false;

	return true;
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
