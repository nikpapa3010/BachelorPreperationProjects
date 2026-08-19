#pragma once

class FHighlighterLogic;

class SBoxHighlight : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SBoxHighlight)
		{
		}

		SLATE_ARGUMENT(float, HighlightWidth)
		SLATE_ARGUMENT(float, HighlightHeight)
		SLATE_ARGUMENT(FHighlighterLogic*, Logic)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	

	void SetWidth(float var) { Width = var; }
	void SetHeight(float var) { Height = var; }

private:
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	                      const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;

	float Width = 0;
	float Height = 0;
	FHighlighterLogic* HighlighterLogic = nullptr;
	SGraphPanel* Parent = nullptr;
	// virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
