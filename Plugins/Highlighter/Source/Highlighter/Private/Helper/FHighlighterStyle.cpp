#include "Helpers/FHighlighterStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FHighlighterStyle::StyleSetName = FName("ReadingBlueprintStyle");
TSharedPtr<FSlateStyleSet> FHighlighterStyle::CreatedSlateStyleSet = nullptr;

void FHighlighterStyle::InitializeIcons()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
}

void FHighlighterStyle::ShutDown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}

TSharedRef<FSlateStyleSet> FHighlighterStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("Highlighter"))->GetBaseDir() / "Resources";

	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon16x16(16.f, 16.f);
	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets",
	                    new FSlateImageBrush(IconDirectory / "DeleteUnusedAsset.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders",
	                    new FSlateImageBrush(IconDirectory / "DeleteEmptyFolders.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.AdvanceDeletion",
	                    new FSlateImageBrush(IconDirectory / "AdvanceDeletion.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.SelectionLock",
	                    new FSlateImageBrush(IconDirectory / "SelectionLock.PNG", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.SelectionUnlock",
	                    new FSlateImageBrush(IconDirectory / "SelectionUnlock.png", Icon16x16));

	const FCheckBoxStyle SelectionLockToggleButtonStyle = FCheckBoxStyle()
	                                                      .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
	                                                      .SetPadding(FMargin(10.f))

	                                                      //Uncheck images
	                                                      .SetUncheckedImage(FSlateImageBrush(IconDirectory / "SelectionLock.PNG", Icon16x16, FStyleColors::White25))
	                                                      .SetUncheckedHoveredImage(FSlateImageBrush(IconDirectory / "SelectionLock.PNG", Icon16x16, FStyleColors::AccentBlue))
	                                                      .SetUncheckedPressedImage(FSlateImageBrush(IconDirectory / "SelectionLock.PNG", Icon16x16, FStyleColors::Foreground))
	                                                      //Checked images
	                                                      .SetCheckedImage(FSlateImageBrush(IconDirectory / "SelectionLock.PNG", Icon16x16, FStyleColors::Foreground))
	                                                      .SetCheckedHoveredImage(FSlateImageBrush(IconDirectory / "SelectionLock.PNG", Icon16x16, FStyleColors::AccentBlack))
	                                                      .SetCheckedPressedImage(FSlateImageBrush(IconDirectory / "SelectionLock.PNG", Icon16x16, FStyleColors::AccentGray));
	CustomStyleSet->Set("SceneOutliner.SelectionLock", SelectionLockToggleButtonStyle);

	return CustomStyleSet;
}
