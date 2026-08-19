#include "View/BoxHighlight.h"
#include "Controller/HighlighterLogic.h"

void SBoxHighlight::Construct(const FArguments& InArgs)
{
	Width = InArgs._HighlightWidth;
	Height = InArgs._HighlightHeight;
	HighlighterLogic = InArgs._Logic;

	HighlighterLogic->CaptureViewOffset();
}

int32 SBoxHighlight::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                             const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("WhiteBrush");


	FPaintGeometry PaintGeom = AllottedGeometry.ToPaintGeometry(
		FVector2D(Width, Height), // Size to draw
		FSlateLayoutTransform() // No additional local offset
	);

	FSlateDrawElement::MakeBox
	(
		OutDrawElements,
		LayerId,
		PaintGeom,
		WhiteBrush,
		ESlateDrawEffect::None,
		FLinearColor(.5f, .5f, .5f, .1f)
	);

	return LayerId;
}

FVector2D SBoxHighlight::ComputeDesiredSize(float) const
{
	return FVector2D(Width, Height);
}


void SBoxHighlight::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	bool bFoundNodes = false;
	HighlighterLogic->GetBounds(bFoundNodes);
	if (!bFoundNodes) return;
	
	// get the bound pos on tick, for again, the graphpanel does not do that for us.
	FVector2D BoundPos = HighlighterLogic->GetBoundPos();
	FMargin NewPadding = FMargin(BoundPos.X - 10.f, BoundPos.Y - 10.f, 0, 0);
	HighlighterLogic->SetOverlayPadding(NewPadding);

	// update the visual size of our boxes on tick, because the overlay does not do that for us.
	HighlighterLogic->UpdateVisualSize();
	Width = HighlighterLogic->HighlightInfo->GetVisualSize().X;
	Height = HighlighterLogic->HighlightInfo->GetVisualSize().Y;
}
