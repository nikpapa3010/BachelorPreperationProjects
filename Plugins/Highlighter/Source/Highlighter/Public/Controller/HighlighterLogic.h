#pragma once
#include "SGraphPanel.h"
#include "Model/HighlightInfo.h"

class FHighlightInfo;

class FHighlighterLogic
{
public:
	void GetBounds(bool& OutbFoundNodes);

	FVector2D GetBoundPos();

	void UpdateVisualSize();
	
	void GetZoomAmount();

	void CaptureViewOffset() { GraphStartPos = HighlightInfo->GetGraphPanel()->GetViewOffset(); }
	
	void SetOverlayPadding(FMargin NewPadding);

	FVector2D GraphStartPos;

	FHighlightInfo* HighlightInfo = new FHighlightInfo();
};
