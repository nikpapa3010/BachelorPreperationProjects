// Copyright Epic Games, Inc. All Rights Reserved.

#include "Highlighter.h"

#include "BlueprintEditor.h"
#include "GraphEditor.h"
#include "BlueprintEditorContext.h"
#include "SGraphPanel.h"
#include "Controller/HighlighterLogic.h"
#include "Helpers/FHighlighterStyle.h"
#include "View/BoxHighlight.h"

#define LOCTEXT_NAMESPACE "FHighlighterModule"

void FHighlighterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FHighlighterStyle::InitializeIcons();


	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(
		this, &FHighlighterModule::InitPBMenuExtention));
}


void FHighlighterModule::InitPBMenuExtention()
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus) return;

	// Menu name for Blueprint Editor Toolbar
	UToolMenu* ToolBarMenu = ToolMenus->ExtendMenu("AssetEditorToolbar.CommonActions");
	if (!ToolBarMenu) return;


	FToolMenuSection& Section = ToolBarMenu->FindOrAddSection("CommonActions");
	FToolMenuEntry& Entry = Section.AddDynamicEntry("HighlightNodes", FNewToolMenuSectionDelegate::CreateLambda([this](FToolMenuSection& InSection)
	{
		//this context means that i can get data from it, e.g. Blueprint editor, this can be like the content and maybe even debug window.
		UBlueprintEditorToolMenuContext* ToolMenuContext = InSection.FindContext<UBlueprintEditorToolMenuContext>();
		if (ToolMenuContext)
		{
			//Creates an execution action for the button to do. 
			FToolUIActionChoice NodeAction(FExecuteAction::CreateLambda([ToolMenuContext, this]()
			{
				HighlighterLogic = new FHighlighterLogic();

				TSharedPtr<FBlueprintEditor> BlueprintEditorPtr = ToolMenuContext->BlueprintEditor.Pin();
				UEdGraph* FocusedGraph = BlueprintEditorPtr->GetFocusedGraph();
				FGraphPanelSelectionSet SelectedNodes = BlueprintEditorPtr->GetSelectedNodes();


				//Getting FocusedGraph as a Slate to allow for Slate logic
				TSharedPtr<SGraphEditor> GraphEditor = BlueprintEditorPtr->OpenGraphAndBringToFront(FocusedGraph, true);
				if (!GraphEditor.IsValid()) return;

				//Getting Pannel for coordinate system acuracy.
				SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();
				if (!GraphPanel) return;

				//The parent of the Graphpanel is an overlay which we can use to add our coment box in it.
				//Using the overlay takes advantage of the existing logic, e.g. Nodes being behind the rest of the ui.
				TSharedPtr<SWidget> Widget = GraphPanel->GetParentWidget();
				TSharedPtr<SOverlay> Overlay = StaticCastSharedPtr<SOverlay>(Widget);

				//Set everything we need.
				TSharedPtr<SWindow> WidgetWindow = FSlateApplication::Get().FindWidgetWindow(GraphPanel->AsShared());
				HighlighterLogic->HighlightInfo->SetOwnerWindow(WidgetWindow);
				HighlighterLogic->HighlightInfo->SetSelectedNodes(SelectedNodes);
				HighlighterLogic->HighlightInfo->SetGraphEditor(GraphEditor);
				HighlighterLogic->HighlightInfo->SetGraphPanel(GraphPanel);
				HighlighterLogic->UpdateVisualSize();
				bool bFoundNodes = false;
				HighlighterLogic->GetBounds(bFoundNodes);
				if (!bFoundNodes) return;


				SOverlay::FOverlaySlot* TempOverlay = nullptr;
				// Apply the highlight on the Graphs overlay.
				Overlay->AddSlot()
				       .VAlign(VAlign_Top)
				       .HAlign(HAlign_Left)
				       .Expose(TempOverlay)
				[
					SNew(SBoxHighlight)
					.HighlightWidth(HighlighterLogic->HighlightInfo->GetVisualSize().X)
					.HighlightHeight(HighlighterLogic->HighlightInfo->GetVisualSize().Y)
					.Logic(HighlighterLogic)
					.Visibility(EVisibility::HitTestInvisible)
				];

				HighlighterLogic->HighlightInfo->SetOverlaySlot(TempOverlay);
			}));

			InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("HighlightNodes"),
			                                                 FText::FromString("Highlights Nodes"),
			                                                 FText::FromString("Highlight Nodes cause why not."),
			                                                 FSlateIcon(FAppStyle::GetAppStyleSetName(),
			                                                            "Icons.Toolbar.Settings"),
			                                                 NodeAction)
			);
		}
	}));
	Entry.InsertPosition = FToolMenuInsert("FindInContentBrowser", EToolMenuInsertType::After);


	Section.AddEntry(Entry);
}

void FHighlighterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHighlighterModule, Highlighter)
