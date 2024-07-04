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
