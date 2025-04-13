// Copyright 2022, Aquanox.
#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/DeveloperSettings.h"
#include "BlueprintMessageSettings.generated.h"

/**
 * Struct that represents messagelog category definition
 */
USTRUCT()
struct FBlueprintMessageLogCategory
{
	GENERATED_BODY()

	/** Name of the custom category. If category was already registered Advanced Settings can be updated. */
	UPROPERTY(EditAnywhere, Category=General)
	FName Name;

	/** Friendly name of the custom category */
	UPROPERTY(EditAnywhere, Category=General)
	FText DisplayName;
	
	/** Name of the custom category */
	UPROPERTY(EditAnywhere, Category=General)
	bool bUseAdvancedSettings = false;

	/** Whether to show the filters menu */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings", EditConditionHides))
	bool bShowFilters = false;

	/**
	 * Whether to initially  show the pages widget. Setting this to false will allow the user to manually clear the log.
	 * If this is not set & NewPage() is called on the log, the pages widget will show itself
	 */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings", EditConditionHides))
	bool bShowPages = false;

	/**
	* Whether to allow the user to clear this log.
	*/
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings", EditConditionHides))
	bool bAllowClear = true;

	/**
	 * Whether to check for duplicate messages & discard them
	 */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings", EditConditionHides))
	bool bDiscardDuplicates = false;

	/** The maximum number of pages this log can hold. Pages are managed in a first in, last out manner */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings", EditConditionHides))
	uint32 MaxPageCount = 20;

	/** Whether to show this log in the main log window */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings", EditConditionHides))
	bool bShowInLogWindow = true;

	/** Whether to scroll to the bottom of the window when messages are added */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings", EditConditionHides))
	bool bScrollToBottom = false;
};

/**
 * Plugin settings class
 */
UCLASS(Config=EditorPerProjectUserSettings, meta=(DisplayName="Blueprint MessageLog Settings"))
class BLUEPRINTMESSAGE_API UBlueprintMessageSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:

	static const UBlueprintMessageSettings* Get()
	{
		return GetDefault<UBlueprintMessageSettings>();
	}

	virtual FName GetContainerName() const override { return "Editor"; }
	virtual FName GetCategoryName() const override { return "Plugins"; }
	virtual FName GetSectionName() const override { return "BlueprintMessageLog"; }

#if WITH_EDITOR
	virtual FText GetSectionText() const override { return INVTEXT("Blueprint Message Log"); }
	virtual FText GetSectionDescription() const override { return INVTEXT("Message Log Blueprint integration plugin settings"); }
#endif

	FName GetDefaultCategory() const;

	// Helper for Default Category Combo
	UFUNCTION()
	TArray<FName> GetDefaultCategoryOptions();
	// Helper for Selectable Categories Combo
	UFUNCTION()
	TArray<FName> GetSelectableCategoryOptions();
	
	void GetAvailableCategories(TArray<FName>& OutCategories) const;

	void DiscoverExistingCategories(TArray<FName>& OutCategories) const;

public:

	// Setting this to true will allow the message log to be displayed when OpenMessageLog is called.
	UPROPERTY(Config, EditAnywhere, Category=General)
	bool bEnableMessageLogDisplay = true;

	// Specify list of selectable message log categories
	// If value is empty combined array of Default + Discovered + Custom is used
	UPROPERTY(Config, EditAnywhere, Category=General, meta=(GetOptions="GetSelectableCategoryOptions", NoElementDuplicate))
	TArray<FName> SelectableCategories;

	// User defined message log categories
	UPROPERTY(Config, EditAnywhere, Category=General, meta=(ConfigRestartRequired=true))
	TArray<FBlueprintMessageLogCategory> CustomCategories;

	// Should attempt to discover existing log categories from engine internals.
	// To use this feature - it has to be enabled in BlueprintMessage.Build.cs
	UPROPERTY(Config, EditAnywhere, Category=Advanced, meta=(ConfigRestartRequired=true))
	bool bDiscoverStandardCategories = true;

	// Discovered categories
	mutable TOptional<TArray<FName>> DiscoveredCategories;

	// Global default category name
	UPROPERTY(Config, EditAnywhere, NoClear, Category=Advanced, meta=(GetOptions="GetDefaultCategoryOptions"))
	FName DefaultCategory = TEXT("BlueprintLog");

	// Global Initial value for bSuppressLoggingToOutputLog for constructed messages
	// If suppressed shown message text won't be duplicated in output log
	// Default is False
	UPROPERTY(Config, EditAnywhere, Category=Advanced)
	bool bDefaultSuppressLoggingToOutputLog = false;

	// Global Initial value for bAutoDestroy for constructed messages
	// If enabled message object will be destroyed after calling Show
	// Default is False
	UPROPERTY(Config, EditAnywhere, Category=Advanced)
	bool bDefaultAutoDestroy = false;

};
