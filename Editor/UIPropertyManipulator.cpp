#include "UIPropertyManipulator.hpp"
#include "Editor.hpp"
#include <Engine/UIComboBox.hpp>
#include <Engine/UICheckbox.hpp>
#include <Engine/UITextButton.hpp>
#include <Engine/FontManager.hpp>
#include <Engine/MaterialManager.hpp>

byte StringToClassID(const String& string)
{
	Buffer<Pair<const byte*, RegistryNodeBase* const*>> types = Engine::Instance().registry.GetRegisteredTypes();
	for (size_t i = 0; i < types.GetSize(); ++i)
	{
		if ((*types[i].second)->name == string)
			return *types[i].first;
	}

	return 0;
}

String ClassIDToString(byte id)
{
	Buffer<Pair<const byte*, RegistryNodeBase* const*>> types = Engine::Instance().registry.GetRegisteredTypes();
	for (size_t i = 0; i < types.GetSize(); ++i)
	{
		if (*types[i].first == id)
			return (*types[i].second)->name;
	}

	return "ERROR";
}

void UIPropertyManipulator::_OnPressed(UIButton& button)
{
	if (!_refreshing)
	{
		String string;
		if (_property.GetFlags() & PropertyFlags::MATERIAL)
		{
			string = _editorInstance.SelectMaterialDialog();
		}
		else
		{
			string = _editorInstance.SelectModelDialog();
		}

		if (string.GetLength() > 0)
		{
			_property.SetAsString(_object, string);
			_textbox->SetString(string);
		}
	}
}

void UIPropertyManipulator::_OnStateChanged(UICheckbox& checkbox)
{
	if (!_refreshing && _property.GetType() == EPropertyType::BOOL)
	{
		VariableProperty<bool>& boolProperty = dynamic_cast<VariableProperty<bool>&>(_property);
		boolProperty.Set(_object, checkbox.GetState());
		_textbox->SetString(checkbox.GetState() ? "True" : "False");
	}
}

String GetPropertyString(Property& property, const void* object)
{
	if (property.GetFlags() & PropertyFlags::CLASSID)
	{
		VariableProperty<byte>* idProperty = dynamic_cast<VariableProperty<byte>*>(&property);
		return ClassIDToString(idProperty->Get(object));
	}
	else
		return property.GetAsString(object);
}

void SetPropertyString(Property& property, void* object, const String& string)
{
	if (property.GetFlags() & PropertyFlags::CLASSID)
	{
		VariableProperty<byte>* idProperty = dynamic_cast<VariableProperty<byte>*>(&property);
		idProperty->Set(object, StringToClassID(string));
	}
	else
		property.SetAsString(object, string);
}

void UIPropertyManipulator::_OnStringChanged(UITextbox& textbox)
{
	if (!_refreshing)
		SetPropertyString(_property, _object, textbox.GetString());
}

void UIPropertyManipulator::_OnStringChanged(UIComboBox& combobox)
{
	if (!_refreshing)
		SetPropertyString(_property, _object, combobox.GetString());
}

UIPropertyManipulator::UIPropertyManipulator(float h, Editor& instance, Property& property, void* object, UIElement* parent) : 
	UIContainer(parent), _editorInstance(instance), _label(this), _object(object), _property(property), _textbox(nullptr), _checkbox(nullptr), _combobox(nullptr), _refreshing(false)
{
	bool readOnly = property.GetFlags() & PropertyFlags::READONLY;

	const float borderSize = 2.f;
	const UIColour buttonColourInactive(Colour::Black, Colour(.6f, .6f, .6f));
	const UIColour buttonColourHover(Colour::Grey, Colour(.8f, .8f, .8f));
	const UIColour buttonColourHold(Colour::Black, Colour(.2f, .2f, .2f));
	const UIColour textColour(readOnly ? Colour::Grey : Colour::White);
	const UIColour textShadowColour(Colour(0.f, 0.f, 0.12f));
	const Vector2 textShadowOffset(2.f, -2.f);

	SetBounds(0.f, 0.f, 1.f, UICoord(0.f, h));

	SharedPointer<const Font> font = Engine::Instance().pFontManager->Get("arial");
	SharedPointer<const Material> btnMat = Engine::Instance().pMaterialManager->Get("uibutton1");

	_label.SetFont(font).SetColour(textColour).SetShadowColour(textShadowColour).SetShadowOffset(textShadowOffset).SetString(_property.GetName());
	_label.SetBounds(0.f, 0.f, .5f, 1.f);

	float tbWidthOffset = 0.f;

	if (!readOnly && (property.GetFlags() & PropertyFlags::MATERIAL || property.GetFlags() & PropertyFlags::MODEL))
	{
		tbWidthOffset = -30.f;

		UITextButton* browse = new UITextButton(this);
		browse->SetBounds(UICoord(1.f, -30.f), 0.f, UICoord(0.f, 30.f), 1.f);
		browse->SetString("...").SetFont(font)
			.SetTextColour(textColour).SetTextShadowColour(textShadowColour).SetTextShadowOffset(textShadowOffset)
			.SetMaterial(btnMat).SetBorderSize(borderSize)
			.SetColour(buttonColourInactive).SetColourHover(buttonColourHover).SetColourHold(buttonColourHold);

		browse->onPressed += FunctionPointer<void, UIButton&>(this, &UIPropertyManipulator::_OnPressed);
	}
	else if (property.GetFlags() & PropertyFlags::DIRECTION || property.GetFlags() & PropertyFlags::CLASSID)
	{
		UIComboBox* comboBox = new UIComboBox(this);
		comboBox->SetBounds(.5f, 0.f, .5f, 1.f);
		comboBox->SetReadOnly(readOnly).SetListFont(font).SetListMaterial(btnMat).SetListBorderSize(borderSize).SetListColour(buttonColourInactive)
			.SetListTextColour(textColour).SetListTextShadowColour(textShadowColour).SetListTextShadowOffset(textShadowOffset).SetListItemHeight(h)
			.SetString(GetPropertyString(_property, _object)).SetFont(font).SetTextColour(textColour).SetTextShadowColour(textShadowColour).SetTextShadowOffset(textShadowOffset)
			.SetMaterial(btnMat).SetBorderSize(borderSize).SetColour(buttonColourInactive).SetColourHover(buttonColourHover).SetColourHold(buttonColourHold);

		if (!readOnly)
		{
			comboBox->onStringChanged += FunctionPointer<void, UIComboBox&>(this, &UIPropertyManipulator::_OnStringChanged);

			if (property.GetFlags() & PropertyFlags::DIRECTION)
			{
				comboBox->Add("north");
				comboBox->Add("east");
				comboBox->Add("south");
				comboBox->Add("west");
			}
			else
			{
				auto types = Engine::Instance().registry.GetRegisteredTypes();
				for (uint32 i = 0; i < types.GetSize(); ++i)
				{
					comboBox->Add((*types[i].second)->name);
				}
			}
		}

		return; //We don't want a textbox, seeya
	}

	if (property.GetType() == EPropertyType::BOOL)
	{
		tbWidthOffset = -h;

		UICheckbox* check = new UICheckbox(this);
		check->SetBounds(UICoord(1.f, -h), 0.f, UICoord(0.f, h), UICoord(0.f, h));
		check->SetReadOnly(readOnly).SetTextureTrue(Engine::Instance().pTextureManager->Get("ui/checkmark")).
			SetMaterial(btnMat).SetBorderSize(borderSize).SetColourFalse(buttonColourInactive).SetColourTrue(buttonColourInactive).SetColourHover(buttonColourHover).SetColourHold(buttonColourHold);
		
		if (!readOnly)
			check->onStateChanged += FunctionPointer<void, UICheckbox&>(this, &UIPropertyManipulator::_OnStateChanged);
	}

	_textbox = new UITextbox(this);
	_textbox->SetBounds(.5f, 0.f, UICoord(.5f, tbWidthOffset), 1.f);
	_textbox->SetReadOnly(readOnly).SetMaterial(btnMat).SetBorderSize(borderSize)
		.SetFont(font).SetTextColour(textColour).SetTextShadowColour(textShadowColour).SetTextShadowOffset(textShadowOffset)
		.SetString(GetPropertyString(_property, _object)).SetColour(buttonColourInactive);

	if (!readOnly)
		_textbox->onStringChanged += FunctionPointer<void, UITextbox&>(this, &UIPropertyManipulator::_OnStringChanged);
}

void UIPropertyManipulator::Refresh()
{
	_refreshing = true;

	if (_textbox)
	{
		_textbox->SetString(GetPropertyString(_property, _object));
	}

	if (_checkbox)
	{
		VariableProperty<bool>* boolProperty = dynamic_cast<VariableProperty<bool>*>(&_property);
		if (boolProperty)
			_checkbox->SetState(boolProperty->Get(_object));
	}

	if (_combobox)
	{
		_combobox->SetString(GetPropertyString(_property, _object));
	}

	_refreshing = false;
}
