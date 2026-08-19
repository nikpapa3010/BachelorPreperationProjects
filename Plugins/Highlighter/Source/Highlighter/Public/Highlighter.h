// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FHighlighterLogic;

class FHighlighterModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	void InitPBMenuExtention();
	virtual void ShutdownModule() override;

	TSharedPtr<SBox> ConstructedDockTab;

	bool bIsCreated = false;

private:
	FHighlighterLogic* HighlighterLogic = nullptr;
};
