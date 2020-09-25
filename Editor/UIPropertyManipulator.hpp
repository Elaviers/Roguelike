#pragma once
#include <ELCore/Property.hpp>
#include <ELUI/Container.hpp>
#include <ELUI/Button.hpp>
#include <ELUI/Checkbox.hpp>
#include <ELUI/ComboBox.hpp>
#include <ELUI/Label.hpp>
#include <ELUI/Panel.hpp>
#include <ELUI/Textbox.hpp>

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
	UIPropertyManipulator(
		const UICoord& y, float height, 
		Editor& instance, 
		Property& property, 
		void* object, 
		UIElement* parent = nullptr, 
		const Buffer<String>& comboBoxOptions = Buffer<String>());

	virtual ~UIPropertyManipulator() {}

	void Refresh();

	void* GetObject() const { return _object; }
	void SetObject(void* object) { _object = object; }

	static float AddPropertiesToContainer(const UICoord& y, float height, Editor& instance, const PropertyCollection& properties, void* object, UIContainer& container)
	{
		const Buffer<Property*>& buffer = properties.GetAll();

		size_t i = 0;
		for (; i < buffer.GetSize(); ++i)
			UIPropertyManipulator* p = new UIPropertyManipulator(
				UICoord(y.relative, y.absolute - height * (i + 1)), height, 
				instance, *buffer[i], object, &container);

		return height * i;
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
