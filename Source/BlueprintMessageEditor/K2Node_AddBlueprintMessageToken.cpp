﻿// Copyright 2022, Aquanox.


#include "K2Node_AddBlueprintMessageToken.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintMessage.h"
#include "BlueprintMessageModule.h"
#include "BlueprintMessageToken.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintNodeStatics.h"
#include "FindInBlueprintManager.h"
#include "K2Node_Knot.h"
#include "KismetCompiler.h"

static const FName PN_Chain(TEXT("Message"));
static const FName PN_Slot(TEXT("Slot"));

#define LOCTEXT_NAMESPACE "BlueprintMessage"

UK2Node_AddBlueprintMessageToken::UK2Node_AddBlueprintMessageToken()
{
	FunctionReference.SetExternalMember(
		GET_FUNCTION_NAME_CHECKED(UBlueprintMessage, AddToken),
		UBlueprintMessage::StaticClass()
	);
}

void UK2Node_AddBlueprintMessageToken::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	UEdGraphPin* ChainIn = CreateSelfPin(GetTargetFunction());
	ChainIn->PinFriendlyName = LOCTEXT( "Target", "Target");

	UEdGraphPin* ChainOut = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, NAME_None, UBlueprintMessage::StaticClass(),	PN_Chain);

	// Create other param pins
	UEdGraphPin* SlotPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, PN_Slot);
	GetDefault<UEdGraphSchema_K2>()->SetPinAutogeneratedDefaultValueBasedOnType(SlotPin);

	// Add factory pins
	UK2Node_CallFunction* DummyFactoryNode = NewObject<UK2Node_CallFunction>(GetGraph());
	DummyFactoryNode->SetFromFunction(GetFactoryFunction());
	DummyFactoryNode->AllocateDefaultPins();

	auto CreatePinCopy = [&](UEdGraphPin* ProtoPin)
	{
		const FEdGraphPinType& ProtoPinType = ProtoPin->PinType;
		FCreatePinParams PinParams;
		PinParams.ContainerType = ProtoPinType.ContainerType;
		PinParams.ValueTerminalType = ProtoPinType.PinValueType;
		UEdGraphPin* Pin = CreatePin(ProtoPin->Direction, ProtoPinType.PinCategory, ProtoPinType.PinSubCategory, ProtoPinType.PinSubCategoryObject.Get(), ProtoPin->PinName, PinParams);
		Pin->PinToolTip = ProtoPin->PinToolTip;
		Pin->PinFriendlyName = ProtoPin->PinFriendlyName;
		Pin->DefaultValue = ProtoPin->DefaultValue;
		Pin->DefaultTextValue = ProtoPin->DefaultTextValue;
		return Pin;
	};

	// Copy param pins
	for (UEdGraphPin* Pin : DummyFactoryNode->Pins)
	{
		if (Pin->Direction == EGPD_Input
			&& Pin->PinName != UEdGraphSchema_K2::PN_Execute
			&& Pin->PinName != UEdGraphSchema_K2::PN_Self
		)
		{
			check(!FindPin(Pin->PinName, Pin->Direction));
			CreatePinCopy(Pin);
		}
	}

	DummyFactoryNode->DestroyNode();
}

UFunction* UK2Node_AddBlueprintMessageToken::GetFactoryFunction() const
{
	UFunction* Function = FactoryReference.ResolveMember<UFunction>(GetBlueprintClassFromNode());
	return Function;
}

FText UK2Node_AddBlueprintMessageToken::GetMenuCategory() const
{
	return GetDefaultCategoryForFunction(GetTargetFunction(), FText::GetEmpty());
}

FText UK2Node_AddBlueprintMessageToken::GetTokenTitle() const
{
	if (UFunction* Function = GetFactoryFunction())
	{
		if (Function->HasMetaData(FMessageTokenFactoryRegistration::MD_TokenFactoryName))
		{
			return FText::FromString(Function->GetMetaData(FMessageTokenFactoryRegistration::MD_TokenFactoryName));
		}
		else
		{
			// MakeURLToken -> Add URL Token
			// SomeToken -> Add Some Token
			FString  AutoName = Function->GetName();
			if (AutoName.StartsWith(TEXT("Make")))
			{
				AutoName = Function->GetName().RightChop(4);
			}
			return FText::FromString(FName::NameToDisplayString(*AutoName, false));
		}
	}
	else
	{
		return FText::FromName(FunctionReference.GetMemberName());
	}
}

FText UK2Node_AddBlueprintMessageToken::GetTooltipText() const
{
	FText NodeText = FText::FromString(GetDefaultTooltipForFunction(GetTargetFunction()));
	FText FactoryText = FText::FromString(GetDefaultTooltipForFunction(GetFactoryFunction()));

	FFormatNamedArguments Args;
	Args.Add(TEXT("Node"), NodeText);
	Args.Add(TEXT("Factory"), FactoryText);
	Args.Add(TEXT("TokenType"), GetTokenTitle());

	return FactoryText;
}

FText UK2Node_AddBlueprintMessageToken::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText TokenName = GetTokenTitle();
	FText ContextString = GetFunctionContextString();

	FFormatNamedArguments Args;
	Args.Add(TEXT("TokenName"), TokenName);

	if(TitleType == ENodeTitleType::FullTitle)
	{
		Args.Add(TEXT("ContextString"), ContextString);
		return FText::Format(LOCTEXT( "AddBlueprintMessageToken_FullTitle", "Add {TokenName}\n{ContextString}"), Args);
	}
	else
	{
		return FText::Format(LOCTEXT( "AddBlueprintMessageToken_Title", "Add {TokenName}"), Args);
	}
}

void UK2Node_AddBlueprintMessageToken::GetMenuActions(FBlueprintActionDatabaseRegistrar& InActionRegistrar) const
{
	const UClass* ActionKey = GetClass();
	if (InActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		TArray<FMessageTokenFactoryRegistration> Registrations;
		FMessageTokenFactoryRegistration::GetRegisteredFactories(Registrations);

		for (FMessageTokenFactoryRegistration const& FactoryRegistration : Registrations)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			check(NodeSpawner != nullptr);
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([FactoryRegistration](UEdGraphNode* Node, bool bIsTemplate)
			{
				UK2Node_AddBlueprintMessageToken* NodeImpl = CastChecked<UK2Node_AddBlueprintMessageToken>(Node);
				NodeImpl->FactoryReference.SetExternalMember(FactoryRegistration.FunctionName, FactoryRegistration.FactoryClass.Get());
			});
			InActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
		}
	}
}

void UK2Node_AddBlueprintMessageToken::AddSearchMetaDataInfo(TArray<FSearchTagDataPair>& OutTaggedMetaData) const
{
	Super::AddSearchMetaDataInfo(OutTaggedMetaData);

	if (UFunction* FactoryFunction = GetFactoryFunction())
	{
		OutTaggedMetaData.Add(FSearchTagDataPair(FFindInBlueprintSearchTags::FiB_NativeName, FText::FromString(FactoryFunction->GetName())));
	}
}

FText UK2Node_AddBlueprintMessageToken::GetFunctionContextString() const
{
	return Super::GetFunctionContextString();
}

void UK2Node_AddBlueprintMessageToken::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	auto LinkPins = [&](UEdGraphPin* A, UEdGraphPin* B)
	{
		if (A && B)
		{
			A->MakeLinkTo(B);
			return true;
		}
		return false;
	};

	auto MovePinLinksToIntermediate = [&](UEdGraphPin* SourcePin, UEdGraphPin* DestPin)
	{
		if (!SourcePin || !DestPin) return false;
		bool bSuccess = CompilerContext.MovePinLinksToIntermediate(*SourcePin, *DestPin).CanSafeConnect();
		check(bSuccess);
		return bSuccess;
	};

	bool bIsErrorFree = true;

	auto SelfPin = FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Input);

	// spawn knot
	auto KnotNode = CompilerContext.SpawnIntermediateNode<UK2Node_Knot>(this, SourceGraph);
	KnotNode->AllocateDefaultPins();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(KnotNode, this);
	// move self to knot
	bIsErrorFree &= MovePinLinksToIntermediate(SelfPin, KnotNode->GetInputPin());

	// spawn factory node
	auto FactoryNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	FactoryNode->FunctionReference = FactoryReference;
	FactoryNode->AllocateDefaultPins();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(FactoryNode, this);

	// transfer factory pins into factory node
	for (UEdGraphPin* FactoryPin : FactoryNode->Pins)
	{
		UEdGraphPin* NodePin = FindPin(FactoryPin->PinName, EGPD_Input);
		if (NodePin && FactoryPin->PinName != UEdGraphSchema_K2::PN_Self)
		{
			bIsErrorFree &=  MovePinLinksToIntermediate(NodePin, FactoryPin);
		}
	}

	// spawn add token node
	auto AddTokenNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	AddTokenNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UBlueprintMessage, AddToken), UBlueprintMessage::StaticClass());
	AddTokenNode->AllocateDefaultPins();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(AddTokenNode, this);

	// set self
	bIsErrorFree &=  LinkPins(KnotNode->GetOutputPin(), AddTokenNode->FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Input));
	// set slot
	bIsErrorFree &=  MovePinLinksToIntermediate(FindPinChecked(PN_Slot, EGPD_Input), AddTokenNode->FindPinChecked(PN_Slot, EGPD_Input));
	// link return value of spawn to addtoken
	bIsErrorFree &=  LinkPins(FactoryNode->GetReturnValuePin(), AddTokenNode->FindPinChecked(TEXT("Token"), EGPD_Input));

	// move execs to intermediate
	bIsErrorFree &=  MovePinLinksToIntermediate(GetExecPin(), AddTokenNode->GetExecPin());
	bIsErrorFree &=  MovePinLinksToIntermediate(GetThenPin(), AddTokenNode->GetThenPin());
	// move chain to knot
	bIsErrorFree &=  MovePinLinksToIntermediate(FindPinChecked(PN_Chain, EGPD_Output), KnotNode->GetOutputPin());

	if (!bIsErrorFree)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "BaseAsyncTask: Internal connection error. @@").ToString(), this);
	}

	BreakAllNodeLinks();
}

void UK2Node_AddBlueprintMessageToken::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	const UBlueprint* Blueprint = GetBlueprint();
	UFunction *Function = GetFactoryFunction();
	if (Function == nullptr)
	{
		FString OwnerName;

		if (Blueprint != nullptr)
		{
			OwnerName = Blueprint->GetName();
			if (UClass* FuncOwnerClass = FunctionReference.GetMemberParentClass(Blueprint->GeneratedClass))
			{
				OwnerName = FuncOwnerClass->GetName();
			}
		}
		FString const FunctName = FunctionReference.GetMemberName().ToString();

		FText const WarningFormat = LOCTEXT("FunctionNotFoundFmt", "Could not find a function named \"{0}\" in '{1}'.\nMake sure '{2}' has been compiled for @@");
		MessageLog.Error(*FText::Format(WarningFormat, FText::FromString(FunctName), FText::FromString(OwnerName), FText::FromString(OwnerName)).ToString(), this);
	}
}

#undef LOCTEXT_NAMESPACE
