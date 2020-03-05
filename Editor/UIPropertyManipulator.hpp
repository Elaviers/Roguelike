#pragma once
#include <Engine/UIContainer.hpp>
#include <Engine/UIButton.hpp>
#include <Engine/UICheckbox.hpp>
#include <Engine/UIComboBox.hpp>
#include <Engine/UILabel.hpp>
#include <Engine/UIPanel.hpp>
#include <Engine/UITextbox.hpp>
#include <Engine/Property.hpp>

class Editor;

class UIPropertyManipulator : public UIContainer
{
	Editor& _editorInstance;

	bool _refreshing;

	void* _object;
	Property& _property;
	
	UILabel _label;

	UITextbox* _textbox;
	UICheckbox* _checkbox;
	UIComboBox* _combobox;

	void _OnStringChanged(UITextbox&);
	void _OnStringChanged(UIComboBox&);

	void _OnPressed(UIButton&);
	void _OnStateChanged(UICheckbox&);

public:
	UIPropertyManipulator(float height, Editor& instance, Property& property, void* object, UIElement* parent = nullptr);
	virtual ~UIPropertyManipulator() {}

	void Refresh();

	void* GetObject() const { return _object; }
	void SetObject(void* object) { _object = object; }

	static void AddPropertiesToContainer(float height, Editor& instance, const PropertyCollection& properties, void* object, UIContainer& container)
	{
		const Buffer<Property*>& buffer = properties.GetAll();

		for (size_t i = 0; i < buffer.GetSize(); ++i)
		{
			UIPropertyManipulator* p = new UIPropertyManipulator(height, instance, *buffer[i], object, &container);
			p->SetBounds(0.f, UICoord(1.f, -height * (i + 1)), 1.f, UICoord(0.f, height));
		}
	}

	static void RefreshPropertiesInContainer(const UIContainer& container)
	{
		for (size_t i = 0; i < container.GetChildren().GetSize(); ++i)
		{
			UIPropertyManipulator* property = dynamic_cast<UIPropertyManipulator*>(container.GetChildren()[i]);
			if (property)
			{
				property->Refresh();
			}
		}
	}

	static void SetObjectOnPropertiesInContainer(const UIContainer& container, void* obj)
	{
		for (size_t i = 0; i < container.GetChildren().GetSize(); ++i)
		{
			UIPropertyManipulator* property = dynamic_cast<UIPropertyManipulator*>(container.GetChildren()[i]);
			if (property)
			{
				property->SetObject(obj);
			}
		}
	}
};
