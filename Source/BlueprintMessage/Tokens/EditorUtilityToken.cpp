// Copyright 2022, Aquanox.

#include "Tokens/EditorUtilityToken.h"

#include "EditorUtilityObject.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"

FEditorUtilityToken::FEditorUtilityToken(const FText& InActionName, const FText& InActionDescription, bool bSingle)
	: FActionToken(InActionName, InActionDescription, FOnActionTokenExecuted::CreateSP(this, &FEditorUtilityToken::Run), bSingle)
{

}

FEditorUtilityWidgetToken::FEditorUtilityWidgetToken(const FSoftClassPath& UtilityWidgetClass, const FText& Description, bool bSingleUse)
	: FEditorUtilityToken(FText::FromString(UtilityWidgetClass.GetAssetName()), Description, bSingleUse), UtilityWidgetClass(UtilityWidgetClass)
{

}

void FEditorUtilityWidgetToken::Run()
{
	if (UClass* WidgetClass = UtilityWidgetClass.TryLoadClass<UEditorUtilityWidget>())
	{
		if (UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(WidgetClass->ClassGeneratedBy))
		{
			UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
			EditorUtilitySubsystem->SpawnAndRegisterTab(EditorWidget);
		}
	}
}

FEditorUtilityTaskToken::FEditorUtilityTaskToken(const FSoftClassPath& UtilityTaskClass, const FText& Description, bool bSingleUse)
	: FEditorUtilityToken(FText::FromString(UtilityTaskClass.GetAssetName()), Description, bSingleUse), UtilityTaskClass(UtilityTaskClass)
{
}

void FEditorUtilityTaskToken::Run()
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	if (UClass* WidgetClass = UtilityTaskClass.TryLoadClass<UEditorUtilityObject>())
	{
		EditorUtilitySubsystem->TryRun(WidgetClass);
	}
}
