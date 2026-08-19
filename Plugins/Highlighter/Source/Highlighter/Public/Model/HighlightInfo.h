#pragma once

class FHighlightInfo
{
	
private:
	//The Nodes we want to highlight
	FGraphPanelSelectionSet SelectedNodes;

	// The Highlight itself and its bounds
	FBox2D Bounds;
	
	TSharedPtr<SGraphEditor> GraphEditor;
	
	SGraphPanel* GraphPanel = nullptr;

	// The actual size of our highlight.
	FVector2D VisualSize;

	// The overlayslot where our highlight is in.
	SOverlay::FOverlaySlot* OverlaySlot = nullptr;
	
	//The window we use to calculate position offset of our graph. TODO: Check for better solution using SGraphPanel
	TSharedPtr<SWindow> OwnerWindow = nullptr;
	
	FVector2D ViewLocation;
	float ZoomAmount = 0;

#pragma region Getters & Setters

public:
	
	void SetSelectedNodes(FGraphPanelSelectionSet var) { SelectedNodes = var; }
	FGraphPanelSelectionSet GetSelectedNodes() { return SelectedNodes; }

	void SetBounds(FBox2D var) { Bounds = var; }
	FBox2D GetBounds() { return Bounds; }

	float GetZoomAmount() const
	{
		return ZoomAmount;
	}

	void SetZoomAmount(float var)
	{
		this->ZoomAmount = var;
	}

	FVector2D GetViewLocation() const
	{
		return ViewLocation;
	}

	void SetViewLocation(const FVector2D& var)
	{
		this->ViewLocation = var;
	}

	TSharedPtr<SGraphEditor> GetGraphEditor() const
	{
		return GraphEditor;
	}

	void SetGraphEditor(const TSharedPtr<SGraphEditor>& var)
	{
		this->GraphEditor = var;
	}

	SGraphPanel* GetGraphPanel() const
	{
		return GraphPanel;
	}

	void SetGraphPanel(SGraphPanel* var)
	{
		this->GraphPanel = var;
	}

	FVector2D GetVisualSize() const
	{
		return VisualSize;
	}

	void SetVisualSize(const FVector2D& var)
	{
		this->VisualSize = var;
	}

	SOverlay::FOverlaySlot* GetOverlaySlot() const
	{
		return OverlaySlot;
	}

	void SetOverlaySlot(SOverlay::FOverlaySlot* var)
	{
		this->OverlaySlot = var;
	}

	TSharedPtr<SWindow> GetOwnerWindow() const
	{
		return OwnerWindow;
	}

	void SetOwnerWindow(TSharedPtr<SWindow> var)
	{
		this->OwnerWindow = var;
	}
#pragma endregion
};
