// Copyright 2022, Aquanox.
#include "BlueprintMessageSettings.h"
#include "BlueprintMessageToken.h"
#include "BlueprintMessage.h"

#if WITH_EDITOR && defined(WITH_MESSAGELOG_HACKS)
#include "MessageLogModule.h"
#include "Presentation/MessageLogViewModel.h"

namespace Magic
{
	template<typename Accessor, typename Accessor::Member Member>
	struct AccessPrivate
	{
		friend typename Accessor::Member GetPrivate(Accessor InAccessor)
		{
			return Member;
		}
	};
}

#define BM_IMPLEMENT_GET_PRIVATE_VAR(InClass, VarName, VarType) \
	struct InClass##VarName##Accessor { \
		using Member = VarType InClass::*; \
		friend Member GetPrivate(InClass##VarName##Accessor); \
	}; \
	template struct Magic::AccessPrivate<InClass##VarName##Accessor, &InClass::VarName>;

class FMessageLogViewModel;
using ViewModelList = TSharedPtr<FMessageLogViewModel>;
class FMessageLogListingViewModel;
using ViewModelNameMap = TMap< FName, TSharedPtr<FMessageLogListingViewModel> >;

BM_IMPLEMENT_GET_PRIVATE_VAR(FMessageLogModule, MessageLogViewModel, ViewModelList);
BM_IMPLEMENT_GET_PRIVATE_VAR(FMessageLogViewModel, NameToViewModelMap, ViewModelNameMap);


#undef BM_IMPLEMENT_GET_PRIVATE_VAR

#endif

const FName UBlueprintMessageSettings::DefaultCategory("BlueprintLog");

void UBlueprintMessageSettings::GetAvailableCategories(TArray<FName>& OutCategories)
{
	if (SelectableCategories.Num())
	{
		OutCategories = SelectableCategories;
		return;
	}

	for (auto& Category : CustomCategories)
	{
		OutCategories.AddUnique(Category.Name);
	}

	if (bDiscoverStandardCategories)
	{
		DiscoverExistingCategories(OutCategories);
	}

	if (!OutCategories.Num())
	{
		OutCategories.Add(DefaultCategory);
	}
}

void UBlueprintMessageSettings::DiscoverExistingCategories(TArray<FName>& OutCategories)
{
	if (bDiscoverStandardCategories && !DiscoveredCategories.IsSet())
	{
		TArray<FName> Keys;

#if WITH_EDITOR && defined(WITH_MESSAGELOG_HACKS)

		FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");

		ViewModelList& ViewModel = MessageLogModule.*GetPrivate(FMessageLogModuleMessageLogViewModelAccessor());
		ViewModelNameMap& ViewModelNames = *ViewModel.*GetPrivate(FMessageLogViewModelNameToViewModelMapAccessor());

		ViewModelNames.GetKeys(Keys);

		for (auto It = Keys.CreateIterator(); It; ++It)
		{
			FString AsString = It->ToString();
			if (AsString.Contains(TEXT("PROTO")))
			{
				It.RemoveCurrent();
			}
		}
#endif
		DiscoveredCategories = MoveTemp(Keys);
	}

	if (DiscoveredCategories.IsSet())
	{
		for (FName Name : DiscoveredCategories.GetValue())
		{
			OutCategories.AddUnique(Name);
		}
	}
}
