// Copyright 2022, Aquanox.
#include "BlueprintMessageSettings.h"
#include "BlueprintMessageToken.h"
#include "BlueprintMessage.h"
#include "Modules/ModuleManager.h"
#include "Misc/EngineVersionComparison.h"

#define WITH_MESSAGELOG_DISCOVERY_LEGACY 0

#ifdef WITH_MESSAGELOG_DISCOVERY
#include "MessageLogModule.h"
#include "Presentation/MessageLogViewModel.h"

class FMessageLogModule;
class FMessageLogViewModel;
class FMessageLogListingViewModel;

// @see FMessageLogModule::MessageLogViewModel
using FViewModelPtr = TSharedPtr<FMessageLogViewModel>; 
// @see FMessageLogViewModel::NameToViewModelMap
using FNameToModelMap = TMap< FName, TSharedPtr<FMessageLogListingViewModel> >;

#if UE_VERSION_OLDER_THAN(5, 5, 0) || WITH_MESSAGELOG_DISCOVERY_LEGACY

template<typename Accessor, typename Accessor::Member Member>
struct AccessPrivate
{
	friend typename Accessor::Member GetPrivate(Accessor InAccessor)
	{
		return Member;
	}
};

#define BM_IMPLEMENT_GET_PRIVATE_VAR(InClass, VarName, VarType) \
	struct InClass##VarName##Accessor { \
		using Member = VarType InClass::*; \
		friend Member GetPrivate(InClass##VarName##Accessor); \
	}; \
	template struct AccessPrivate<InClass##VarName##Accessor, &InClass::VarName>;

BM_IMPLEMENT_GET_PRIVATE_VAR(FMessageLogModule, MessageLogViewModel, FViewModelPtr);
BM_IMPLEMENT_GET_PRIVATE_VAR(FMessageLogViewModel, NameToViewModelMap, FNameToModelMap);
#undef BM_IMPLEMENT_GET_PRIVATE_VAR

#else // UE_VERSION_OLDER_THAN(5, 5, 0)
#include "Misc/DefinePrivateMemberPtr.h"

UE_DEFINE_PRIVATE_MEMBER_PTR(FViewModelPtr, GFViewModelPtr, FMessageLogModule, MessageLogViewModel);
UE_DEFINE_PRIVATE_MEMBER_PTR(FNameToModelMap, GFNameToModelMap, FMessageLogViewModel, NameToViewModelMap);
#endif // UE_VERSION_OLDER_THAN(5, 5, 0)

#endif // WITH_MESSAGELOG_DISCOVERY

FName UBlueprintMessageSettings::GetDefaultCategory() const
{
	return DefaultCategory.IsNone() ? TEXT("BlueprintLog") : DefaultCategory;
}

TArray<FName> UBlueprintMessageSettings::GetDefaultCategoryOptions()
{
	TArray<FName> Result;
	Result.Add(TEXT("BlueprintLog"));
	for (auto& Category : CustomCategories)
	{
		Result.AddUnique(Category.Name);
	}
	if (bDiscoverStandardCategories)
	{
		DiscoverExistingCategories(Result);
	}
	return Result;
}

TArray<FName> UBlueprintMessageSettings::GetSelectableCategoryOptions()
{
	TArray<FName> Result;
	Result.Append(GetDefaultCategoryOptions());
	Result.RemoveAll([this](const FName& InName) { return SelectableCategories.Contains(InName); });
	return Result;
}

void UBlueprintMessageSettings::GetAvailableCategories(TArray<FName>& OutCategories) const
{
	OutCategories.Empty();
	OutCategories.Add(NAME_None);
	
	if (SelectableCategories.Num() != 0)
	{
		// allow user configureation specify anything other than default value
		// including placemnent of default category. order is preserved to match settings
		for (auto& Category : SelectableCategories)
		{
			OutCategories.AddUnique(Category);
		}
	}
	else
	{
		TArray<FName> TempCategories;
		TempCategories.AddUnique(GetDefaultCategory());
		// force blueprint log category even if discovery is off
		TempCategories.AddUnique(TEXT("BlueprintLog"));
		
		if (bDiscoverStandardCategories)
		{
			DiscoverExistingCategories(TempCategories);
		}
		
		for (auto& Category : CustomCategories)
		{
			TempCategories.AddUnique(Category.Name);
		}
		
		TempCategories.Sort([](const FName& Left, const FName& Right)
		{
			return Left.Compare(Right) < 0;
		});
		
		OutCategories.Append(MoveTemp(TempCategories));
	}
}

void UBlueprintMessageSettings::DiscoverExistingCategories(TArray<FName>& OutCategories) const
{
#ifdef WITH_MESSAGELOG_DISCOVERY
	if (!DiscoveredCategories.IsSet())
	{
		FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");

#if UE_VERSION_OLDER_THAN(5, 5, 0) || WITH_MESSAGELOG_DISCOVERY_LEGACY
		FViewModelPtr& ViewModel = MessageLogModule.*GetPrivate(FMessageLogModuleMessageLogViewModelAccessor());
		FNameToModelMap& ViewModelNames = *ViewModel.*GetPrivate(FMessageLogViewModelNameToViewModelMapAccessor());
#else
		FViewModelPtr& ViewModel = MessageLogModule.*GFViewModelPtr;
		FNameToModelMap& ViewModelNames = *ViewModel.*GFNameToModelMap;
#endif 

		TArray<FName> Keys;
		ViewModelNames.GetKeys(Keys);

		for (auto It = Keys.CreateIterator(); It; ++It)
		{
			FString AsString = It->ToString();
			if (AsString.Contains(TEXT("PROTO")))
			{
				It.RemoveCurrent();
			}
		}
		
		DiscoveredCategories = MoveTemp(Keys);
	}
#endif // WITH_MESSAGELOG_DISCOVERY
	
	if (DiscoveredCategories.IsSet())
	{
		for (const FName& Name : DiscoveredCategories.GetValue())
		{
			OutCategories.AddUnique(Name);
		}
	}
}
