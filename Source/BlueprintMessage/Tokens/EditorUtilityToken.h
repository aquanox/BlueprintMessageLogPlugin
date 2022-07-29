// Copyright 2022, Aquanox.

#pragma once

#include "Logging/TokenizedMessage.h"

/* Custom token that launches editor utility widget on click */
class FEditorUtilityToken : public FActionToken
{
public:
	FEditorUtilityToken(const FText& InActionName, const FText& InActionDescription, bool bSingle);
protected:
	virtual void Run() = 0;
};

class FEditorUtilityWidgetToken final : public FEditorUtilityToken
{
	FSoftClassPath UtilityWidgetClass;
public:
	FEditorUtilityWidgetToken(const FSoftClassPath& UtilityWidgetClass, const FText& Description, bool bSingleUse);
	virtual void Run() override;
};

class FEditorUtilityTaskToken final : public FEditorUtilityToken
{
	FSoftClassPath UtilityTaskClass;
public:
	FEditorUtilityTaskToken(const FSoftClassPath& UtilityTaskClass, const FText& Description, bool bSingleUse);
	virtual void Run() override;
};