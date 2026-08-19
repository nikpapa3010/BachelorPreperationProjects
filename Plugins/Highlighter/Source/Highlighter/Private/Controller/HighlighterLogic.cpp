#include "Controller/HighlighterLogic.h"

void FHighlighterLogic::GetBounds(bool& OutbFoundNodes)
{
	FBox2D TempBounds(ForceInitToZero);

	for (UObject* SelectedNode : HighlightInfo->GetSelectedNodes())
	{
		UEdGraphNode* EdGraphNode = Cast<UEdGraphNode>(SelectedNode);

		if (EdGraphNode)
		{
			FVector2D CurrentNodePos = FVector2D(EdGraphNode->GetNodePosX(), EdGraphNode->GetNodePosY());
			// FVector2D CurrentNodeWidth = FVector2D(EdGraphNode->GetWidth(), EdGraphNode->GetHeight());
			EdGraphNode->SetHeight(1.f);
			TempBounds += CurrentNodePos; // expands Min/Max automatically
			TempBounds += CurrentNodePos + FVector2D(EdGraphNode->GetWidth() + 250.f, EdGraphNode->GetWidth() + 150.f);
			OutbFoundNodes = true;
		}
	}
	HighlightInfo->SetBounds(TempBounds);
}

FVector2D FHighlighterLogic::GetBoundPos()
{
	GetZoomAmount();

	// Graph space -> Panel local space
	FVector2D PanelMin = (HighlightInfo->GetBounds().Min - HighlightInfo->GetViewLocation()) * HighlightInfo->GetZoomAmount();

	return HighlightInfo->GetOwnerWindow()->GetWindowGeometryInWindow().AbsoluteToLocal(PanelMin);
}

void FHighlighterLogic::UpdateVisualSize()
{
	GetZoomAmount();

	// Convert Graph Space (Min/Max) to Panel Local Space
	FVector2D PanelMin = (HighlightInfo->GetBounds().Min - HighlightInfo->GetViewLocation()) * HighlightInfo->GetZoomAmount();
	FVector2D PanelMax = (HighlightInfo->GetBounds().Max - HighlightInfo->GetViewLocation()) * HighlightInfo->GetZoomAmount();
	HighlightInfo->SetVisualSize(PanelMax - PanelMin);
}

void FHighlighterLogic::GetZoomAmount()
{
	FVector2D ViewLocation;
	float ZoomAmount;
	HighlightInfo->GetGraphEditor()->GetViewLocation(ViewLocation, ZoomAmount);
	HighlightInfo->SetViewLocation(ViewLocation);
	HighlightInfo->SetZoomAmount(ZoomAmount);
}

void FHighlighterLogic::SetOverlayPadding(FMargin NewPadding)
{
	HighlightInfo->GetOverlaySlot()->SetPadding(NewPadding);

}
