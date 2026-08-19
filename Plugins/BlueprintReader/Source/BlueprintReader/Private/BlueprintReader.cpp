// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintReader.h"

#include "BlueprintEditor.h"
#include "BlueprintEditorContext.h"
#include "Helpers/DebugHeader.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilityWidgetComponents.h"
#include "Controller/NodeReading.h"
#include "Helpers/FReadingBlueprintStyle.h"
#include "Model/USnapshotItemData.h"
#include "View/PopUPWindow.h"
#include "View/SnapshotTitlePopUp.h"


#define LOCTEXT_NAMESPACE "FReadingBlueprintTestModule"

class UEditorUtilitySubsystem;

void FBlueprintReaderModule::StartupModule()
{
	FReadingBlueprintStyle::InitializeIcons();

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(
		this, &FBlueprintReaderModule::InitPBMenuExtention));
}

#pragma region BluepritnReadingExtention


void FBlueprintReaderModule::InitPBMenuExtention()
{
	ReadingHelper = new NodeReading();
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus) return;

	// Menu name for Blueprint Editor Toolbar
	UToolMenu* ToolBarMenu = ToolMenus->ExtendMenu("AssetEditorToolbar.CommonActions");
	if (!ToolBarMenu) return;


	FToolMenuSection& Section = ToolBarMenu->FindOrAddSection("CommonActions");

	FToolMenuEntry& NodeReadEntry = CreateGetNodesButtonEntry(Section);

	FToolMenuEntry& CreateSnapshotEntry = CreateSnapshotMenuEntry(Section);


	Section.AddEntry(NodeReadEntry);

	Section.AddEntry(CreateSnapshotEntry);
}


FToolMenuEntry& FBlueprintReaderModule::CreateGetNodesButtonEntry(FToolMenuSection& Section)
{
	FToolMenuEntry& Entry = Section.AddDynamicEntry("GetNodesButton", FNewToolMenuSectionDelegate::CreateLambda([this](FToolMenuSection& InSection)
	{
		//this context means that i can get data from it, e.g. Blueprint editor, this can be like the content and maybe even debug window.
		UBlueprintEditorToolMenuContext* Context = InSection.FindContext<UBlueprintEditorToolMenuContext>();
		if (Context)
		{
			//Creates an execution action for the button to do. 
			FToolUIActionChoice NodeAction(FExecuteAction::CreateLambda([Context, this]()
			{
				UEditorUtilityWidgetBlueprint* WidgetBP = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr, TEXT("/BlueprintReader/UI/BlueprintReaderSettings"));


				if (!WidgetBP) return;

				UEditorUtilitySubsystem* EUS = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
				EUS->SpawnAndRegisterTab(WidgetBP);

				UEditorUtilityWidget* WidgetInstance = EUS->FindUtilityWidgetFromBlueprint(WidgetBP);

				UPopUPWindow* PopUp = Cast<UPopUPWindow>(WidgetInstance);
				if (PopUp)
				{
					PopUp->OnOptionSelected.BindRaw(this, &FBlueprintReaderModule::HandleNodeRead, Context);
					PopUp->OnDeleteAllSnapshots.BindRaw(this, &FBlueprintReaderModule::DeleteAllSnapshots, PopUp);
					PopUp->OnDeleteSelectedSnapshots.BindRaw(this, &FBlueprintReaderModule::DeleteSelectedSnapshots, PopUp);

					PopUp->SnapshotHandler = &SnapshotHandler;
					PopUp->SnapshotHandler->OwnerList = PopUp->SnapshotList;

					for (const FSnapshot& Saved : SnapshotsSaved)
					{
						ImplementNewData(PopUp, Saved);
					}
				}
			}));

			InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("GetNodesButton"),
			                                                 FText::FromString("Get Nodes"),
			                                                 FText::FromString("Gets nodes from this BP"),
			                                                 FSlateIcon(FAppStyle::GetAppStyleSetName(),
			                                                            "Icons.Toolbar.Settings"),
			                                                 NodeAction)
			);
		}
	}));
	Entry.InsertPosition = FToolMenuInsert("FindInContentBrowser", EToolMenuInsertType::After);

	return Entry;
}


void FBlueprintReaderModule::ReadEditorValues(UBlueprintEditorToolMenuContext* Context, UEditorUtilityEditableText* SnapshotTitle)
{
	UBlueprint* Blueprint = Context->GetBlueprintObj();
	if (!ensure(Blueprint != nullptr)) return;

	UObject* ObjectBeingDebugged = Blueprint->GetObjectBeingDebugged();
	if (!ensure(ObjectBeingDebugged != nullptr)) return;
	AActor* ActorInstance = Cast<AActor>(ObjectBeingDebugged); // the live instance, not the CDO
	UClass* Class = ActorInstance->GetClass();

	//Iteratge through all variables the actor has, "None" Flag is used to not go through parents as 
	for (TFieldIterator<FProperty> PropIt(Class, EFieldIterationFlags::None); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		FString ValueAsString;
		Property->ExportTextItem_Direct(ValueAsString, Property->ContainerPtrToValuePtr<void>(ActorInstance), nullptr, ActorInstance, PPF_None);

		FSavedValue Info;

		Info.ValueName = Property->GetName();
		Info.Value = ValueAsString;

		SavedValues.Add(Info);

		DebugHeader::PrintLog(Property->GetID().ToString());
		UE_LOG(LogTemp, Log, TEXT("%s = %s"), *Property->GetName(), *ValueAsString);
	}

	FSnapshot NewSnapshot;
	NewSnapshot.ValueSnapshots = SavedValues;
	NewSnapshot.SnapshotID = FGuid::NewGuid();
	NewSnapshot.SnapshotTitle = SnapshotTitle->GetText().ToString();
	DebugHeader::PrintLog(NewSnapshot.SnapshotTitle);
	SnapshotsSaved.Add(NewSnapshot);
}


void FBlueprintReaderModule::HandleNodeRead(E_SelectedOption SelectedOption, UBlueprintEditorToolMenuContext* Context)
{
	//The blueprint we are in.
	UBlueprint* Blueprint = Context->GetBlueprintObj();
	ReadingHelper->NodeConnections.Empty();


	if (!Blueprint)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No Blueprint found"));
		return;
	}
	TSharedPtr<FBlueprintEditor> BlueprintEditorPtr = Context->BlueprintEditor.Pin();

	FString FinalOutput;
	//Variable to store all nodes.
	TArray<UEdGraphNode*> NodesInGraph;

	// Get nodes from current graph.
	if (BlueprintEditorPtr->GetFocusedGraph())
		BlueprintEditorPtr->GetFocusedGraph()->GetNodesOfClass(NodesInGraph);
	else return;
	USnapshotItemData* ItemData = nullptr;
	USnapshotItem* CurrentSelectedItem = SnapshotHandler.GetCurrentSelectedItem();
	if (CurrentSelectedItem)
		ItemData = CurrentSelectedItem->SnapshotData;
	switch (SelectedOption)
	{
	case E_SelectedOption::ESP_SelectedNode:
		if (BlueprintEditorPtr->GetSingleSelectedNode())
		{
			if (CurrentSelectedItem == nullptr)
				ReadingHelper->NodeTraversal(BlueprintEditorPtr->GetSingleSelectedNode());
			else
				ReadingHelper->NodeTraversalWithValueRead(BlueprintEditorPtr->GetSingleSelectedNode(), Blueprint, ItemData);

			for (auto& Elem : ReadingHelper->NodeConnections)
			{
				if (!Elem.Value.IsEmpty())
					FinalOutput.Append(Elem.Value + "\n");
			}
		}
		else
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok, "No Node selected!");
			return;
		}
		break;
	case E_SelectedOption::ESP_MultipleNodes:
		{
			FGraphPanelSelectionSet Nodes = BlueprintEditorPtr->GetSelectedNodes();

			for (UObject* SelectedNode : Nodes)
			{
				UEdGraphNode* EdGraphNode = Cast<UEdGraphNode>(SelectedNode);
				if (EdGraphNode == nullptr)
				{
					DebugHeader::PrintLog("Cast to graph node failed");
					continue;
				}
				if (CurrentSelectedItem == nullptr)
					ReadingHelper->NodeTraversal(EdGraphNode);
				else
					ReadingHelper->NodeTraversalWithValueRead(EdGraphNode, Blueprint, ItemData);
			}
			for (auto& Elem : ReadingHelper->NodeConnections)
			{
				if (!Elem.Value.IsEmpty())
					FinalOutput.Append(Elem.Value + "\n");
			}
			break;
		}
	case E_SelectedOption::ESP_EventNodes:
		for (UEdGraphNode* EdGraphNode : NodesInGraph)
		{
			FString OriginalNodeName = EdGraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
			if (OriginalNodeName.Contains("event"))
			{
				// NodeNames.Add(OriginalNodeName);
				if (CurrentSelectedItem == nullptr)
					ReadingHelper->NodeTraversal(EdGraphNode);
				else
					ReadingHelper->NodeTraversalWithValueRead(EdGraphNode, Blueprint, ItemData);
			}
		}

		for (auto& Elem : ReadingHelper->NodeConnections)
		{
			if (!Elem.Value.IsEmpty())
				FinalOutput.Append(Elem.Value + "\n");
		}
		break;
	case E_SelectedOption::ESP_Max:
		break;
	default: ;
	}

	DebugHeader::ShowMsgDialog(EAppMsgType::Ok, "Found Nodes \n" + FinalOutput);
}

void FBlueprintReaderModule::ImplementNewData(UPopUPWindow* PopUpWindow, const FSnapshot& SavedSnapshot)
{
	// Create the DATA object, not the widget
	USnapshotItemData* NewData = NewObject<USnapshotItemData>(PopUpWindow);

	NewData->SetSnapshotTitle(SavedSnapshot.SnapshotTitle.IsEmpty() ? TEXT("Test title") : SavedSnapshot.SnapshotTitle);
	NewData->SetSnapshotID(SavedSnapshot.SnapshotID);
	NewData->SetOwnerWindow(PopUpWindow);
	NewData->SetValues(SavedSnapshot.ValueSnapshots);

	PopUpWindow->SnapshotList->AddItem(NewData);
}

#pragma endregion

#pragma region Snapshot

FToolMenuEntry& FBlueprintReaderModule::CreateSnapshotMenuEntry(FToolMenuSection& Section)
{
	FToolMenuEntry& CreateSnapshotEntry = Section.AddDynamicEntry("CreateSnapshotButton", FNewToolMenuSectionDelegate::CreateLambda([this](FToolMenuSection& InSection)
	{
		//this context means that i can get data from it, e.g. Blueprint editor, this can be like the content and maybe even debug window.
		UBlueprintEditorToolMenuContext* Context = InSection.FindContext<UBlueprintEditorToolMenuContext>();


		// We are using ToolUIAction instead to allow for custom availability.
		FToolUIAction SnapshotAction;
		SnapshotAction.ExecuteAction = FToolMenuExecuteAction::CreateLambda([Context, this](const FToolMenuContext& MenuContext)
		{
			UEditorUtilityWidgetBlueprint* WidgetBP = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr, TEXT("/BlueprintReader/UI/EUW_SnapshotPopUp"));


			if (!WidgetBP) return;

			UEditorUtilitySubsystem* EUS = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
			EUS->SpawnAndRegisterTab(WidgetBP);

			UEditorUtilityWidget* WidgetInstance = EUS->FindUtilityWidgetFromBlueprint(WidgetBP);

			USnapshotTitlePopUp* SnapshotTitlePopUp = Cast<USnapshotTitlePopUp>(WidgetInstance);
			if (SnapshotTitlePopUp)
			{
				SnapshotTitlePopUp->OnCompleted.BindRaw(this, &FBlueprintReaderModule::ReadEditorValues, Context, SnapshotTitlePopUp->SnapshotTitle);
			}
		});

		//Allow the Button to be able to get executed only if Playworld is active (aka. user is playing the level)
		SnapshotAction.CanExecuteAction = FToolMenuCanExecuteAction::CreateLambda([](const FToolMenuContext& MenuContext)
		{
			return GEditor != nullptr && GEditor->PlayWorld != nullptr;
		});

		InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("Create Snapshot of Values"),
		                                                 FText::FromString("Create Snapshot"),
		                                                 FText::FromString("Creates snapshot of the current BPs values in the current PIE "),
		                                                 FSlateIcon(FAppStyle::GetAppStyleSetName(),
		                                                            "Symbols.SearchGlass"),

		                                                 SnapshotAction)
		);
	}));
	CreateSnapshotEntry.InsertPosition = FToolMenuInsert("GetNodesButton", EToolMenuInsertType::After);

	return CreateSnapshotEntry;
}


void FBlueprintReaderModule::DeleteAllSnapshots(UPopUPWindow* PopUp)
{
	SnapshotsSaved.Empty();
	// Clear the UI's internal item list (visuals)
	if (PopUp && PopUp->SnapshotList)
	{
		PopUp->SnapshotList->ClearListItems();

		// RequestRefresh is usually redundant after ClearListItems, 
		// but it doesn't hurt for safety in some custom Slate scenarios.
		PopUp->SnapshotList->RequestRefresh();
		TArray<UObject*> SelectedItems;
		PopUp->SnapshotList->GetSelectedItems(SelectedItems);
	}
}

void FBlueprintReaderModule::DeleteSelectedSnapshots(UPopUPWindow* PopUp)
{
	if (PopUp && PopUp->SnapshotList)
	{
		USnapshotItem* CurrentSelectedItem = PopUp->SnapshotHandler->GetCurrentSelectedItem();
		if (CurrentSelectedItem)
		{
			for (FSnapshot& Saved : SnapshotsSaved)
			{
				if (Saved.SnapshotID == CurrentSelectedItem->SnapshotData->GetSnapshotID())
				{
					SnapshotsSaved.RemoveSingle(Saved);
				}
			}
			PopUp->SnapshotList->ClearListItems();

			for (FSnapshot& Saved : SnapshotsSaved)
			{
				ImplementNewData(PopUp, Saved);
			}
			PopUp->SnapshotList->RequestRefresh();
		}
	}
}

#pragma endregion


void FBlueprintReaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FReadingBlueprintStyle::ShutDown();
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprintReaderModule, BlueprintReader)
