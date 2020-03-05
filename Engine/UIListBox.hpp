#pragma once
#include "UIElement.hpp"
#include "UIPanel.hpp"
#include "UILabel.hpp"
#include "UIRect.hpp"
#include "List.hpp"

class UIListBox : public UIElement
{
	UIPanel _panel;
	List<UILabel> _labels;

	UILabel* _selectedLabel;
	UIRect _selectionBox;

	SharedPointer<const Font> _font;
	UIColour _textColour;
	UIColour _textShadowColour;
	Vector2 _textShadowOffset;
	float _itemHeight;

	void _UpdateLabels();

	virtual void _OnBoundsChanged() override
	{
		_panel.UpdateAbsoluteBounds();
		for (auto it = _labels.First(); it.IsValid(); ++it)
			it->UpdateAbsoluteBounds();
	}

public:
	Event<UILabel&> onSelectionChanged;
	Event<UILabel&> onSelectionSubmitted;

	UIListBox(UIElement* parent = nullptr) : UIElement(parent), _panel(this), _selectionBox(this), _selectedLabel(nullptr), _itemHeight(32.f) 
	{
		SetSelectionColour(Colour(0.f, .2f, 1.f, .2f));
	}
	virtual ~UIListBox() {}

	const List<UILabel>& GetItems() const { return _labels; }

	void Add(const String& item);

	float GetBorderSize() const { return _panel.GetBorderSize(); }
	const SharedPointer<const Material>& GetMaterial() const { return _panel.GetMaterial(); }
	const UIColour& GetColour() const { return _panel.GetColour(); }
	float GetItemHeight() const { return _itemHeight; }
	const SharedPointer<const Font>& GetFont() const { return _font; }
	const UIColour& GetTextColour() const { return _textColour; }
	const UIColour& GetTextShadowColour() const { return _textShadowColour; }
	const Vector2& GetTextShadowOffset() const { return _textShadowOffset; }
	const UILabel* GetSelectedLabel() const { return _selectedLabel; }
	const UIColour& GetSelectionColour() const { return _selectionBox.GetColour(); }

	UIListBox& SetBorderSize(float borderSize) { _panel.SetBorderSize(borderSize); return *this; }
	UIListBox& SetMaterial(const SharedPointer<const Material>& material) { _panel.SetMaterial(material); return *this; }
	UIListBox& SetColour(const UIColour& colour) { _panel.SetColour(colour); return *this; }
	UIListBox& SetItemHeight(float itemHeight) { _itemHeight = itemHeight; _UpdateLabels(); return *this; }
	UIListBox& SetFont(const SharedPointer<const Font>& font) { _font = font; _UpdateLabels(); return *this; }
	UIListBox& SetTextColour(const UIColour& colour) { _textColour = colour; _UpdateLabels(); return *this; }
	UIListBox& SetTextShadowColour(const UIColour& colour) { _textShadowColour = colour; _UpdateLabels(); return *this; }
	UIListBox& SetTextShadowOffset(const Vector2& offset) { _textShadowOffset = offset; _UpdateLabels(); return *this; }
	UIListBox& SetSelectionColour(const UIColour& colour) { _selectionBox.SetColour(colour); return *this; }

	virtual void Render() const override
	{
		_panel.Render();
		for (auto it = _labels.First(); it.IsValid(); ++it)
			it->Render();

		if (_selectedLabel)
			_selectionBox.Render();
	}

	virtual bool OnMouseUp() override;

	virtual void OnMouseMove(float mouseX, float mouseY) override;
};
