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
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings"))
	bool bShowFilters = false;

	/**
	 * Whether to initially  show the pages widget. Setting this to false will allow the user to manually clear the log.
	 * If this is not set & NewPage() is called on the log, the pages widget will show itself
	 */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings"))
	bool bShowPages = false;

	/**
	* Whether to allow the user to clear this log.
	*/
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings"))
	bool bAllowClear = true;

	/**
	 * Whether to check for duplicate messages & discard them
	 */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings"))
	bool bDiscardDuplicates = false;

	/** The maximum number of pages this log can hold. Pages are managed in a first in, last out manner */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings"))
	uint32 MaxPageCount = 20;

	/** Whether to show this log in the main log window */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings"))
	bool bShowInLogWindow = true;

	/** Whether to scroll to the bottom of the window when messages are added */
	UPROPERTY(EditAnywhere, Category=Advanced, meta=(EditCondition="bUseAdvancedSettings"))
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

	static const FName DefaultCategory;

	static UBlueprintMessageSettings* Get()
	{
		return GetMutableDefault<UBlueprintMessageSettings>();
	}

	virtual FName GetContainerName() const override { return "Editor"; }
	virtual FName GetCategoryName() const override { return "Plugins"; }
	virtual FName GetSectionName() const override { return "BlueprintMessageLog"; }

#if WITH_EDITOR
	virtual FText GetSectionText() const override { return INVTEXT("Blueprint Message Log"); }
	virtual FText GetSectionDescription() const override { return INVTEXT("Message Log Blueprint integration plugin settings"); }
#endif

	void GetAvailableCategories(TArray<FName>& OutCategories);

	void DiscoverExistingCategories(TArray<FName>& OutCategories);

public:

	// Setting this to true will allow the message log to be displayed when OpenMessageLog is called.
	UPROPERTY(Config, EditAnywhere, Category=General)
	bool bEnableMessageLogDisplay = false;

	// Should attempt to discover existing log categories from internals.
	UPROPERTY(Config, EditAnywhere, Category=General)
	bool bDiscoverStandardCategories = true;
	// Discovered categories
	TOptional<TArray<FName>> DiscoveredCategories;

	// Specify list of selectable message log categories
	// If value is empty set (Custom + Discovered is used)
	UPROPERTY(Config, EditAnywhere, Category=General)
	TArray<FName> SelectableCategories;

	// User defined message log categories
	UPROPERTY(Config, EditAnywhere, Category=General, meta=(ConfigRestartRequired))
	TArray<FBlueprintMessageLogCategory> CustomCategories;

};
