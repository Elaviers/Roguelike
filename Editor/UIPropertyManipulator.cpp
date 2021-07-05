#include "UIPropertyManipulator.hpp"
#include "Editor.hpp"
#include <ELCore/TextProvider.hpp>
#include <ELGraphics/FontManager.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELUI/ComboBox.hpp>
#include <ELUI/Checkbox.hpp>
#include <ELUI/TextButton.hpp>

EObjectID StringToClassID(const String& string, EngineInstance& engine)
{
	const List<ObjectType>& types = engine.objectTypes.GetTypes();
	for (const ObjectType& type : types)
		if (type.GetName() == string)
			return type.GetID();

	return EObjectID::NONE;
}

String ClassIDToString(EObjectID id, EngineInstance& engine)
{
	const List<ObjectType>& types = engine.objectTypes.GetTypes();
	for (const ObjectType& type : types)
		if (type.GetID() == id)
			return type.GetName();

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
			_property.SetAsString(_object, string, _editorInstance.engine.context);
			_textbox->SetText(Text(string));
		}
	}
}

void UIPropertyManipulator::_OnStateChanged(UICheckbox& checkbox)
{
	if (!_refreshing)
	{
		VariableProperty<bool>* boolProperty = dynamic_cast<VariableProperty<bool>*>(&_property);
		if (boolProperty)
		{
			boolProperty->Set(_object, checkbox.GetState(), _editorInstance.engine.context);
			_textbox->SetText(Text(checkbox.GetState() ? "True" : "False"));
		}
	}
}

String GetPropertyString(Property& property, const void* object, EngineInstance& engine)
{
	if (property.GetFlags() & PropertyFlags::CLASSID)
	{
		VariableProperty<byte>* idProperty = dynamic_cast<VariableProperty<byte>*>(&property);
		return ClassIDToString((EObjectID)idProperty->Get(object, engine.context), engine);
	}
	else
		return property.GetAsString(object, engine.context);
}

void SetPropertyString(Property& property, void* object, const String& string, EngineInstance& engine)
{
	if (property.GetFlags() & PropertyFlags::CLASSID)
	{
		VariableProperty<byte>* idProperty = dynamic_cast<VariableProperty<byte>*>(&property);
		idProperty->Set(object, (byte)StringToClassID(string, engine), engine.context);
	}
	else
		property.SetAsString(object, string, engine.context);
}

void UIPropertyManipulator::_OnTextboxStringChanged(UITextbox& textbox)
{
	if (!_refreshing)
		SetPropertyString(_property, _object, textbox.GetText().ToString(), _editorInstance.engine);
}

void UIPropertyManipulator::_OnComboboxStringChanged(UIComboBox& combobox)
{
	if (!_refreshing)
		SetPropertyString(_property, _object, combobox.GetText().ToString(), _editorInstance.engine);
}

UIPropertyManipulator::UIPropertyManipulator(const UICoord& y, float h, Editor& instance, Property& property, void* object, UIElement* parent, const Buffer<Text>& comboText) : 
	UIContainer(parent), _editorInstance(instance), _label(this), _object(object), _property(property), _textbox(nullptr), _checkbox(nullptr), _combobox(nullptr), _refreshing(false)
{
	bool readOnly = property.GetFlags() & PropertyFlags::READONLY;

	const float borderSize = 2.f;
	const UIColour buttonColourInactive(Colour(.6f, .6f, .6f), Colour::Black, Colour::Black);
	const UIColour buttonColourHover(Colour(.8f, .8f, .8f), Colour::Black, Colour::Grey);
	const UIColour buttonColourHold(Colour(.2f, .2f, .2f), Colour::Black, Colour::Black);
	const UIColour textboxColour(Colour(.6f, .6f, .6f), Colour::Black, Colour(.5f, .5f, .5f));
	const UIColour textColour(readOnly ? Colour::Grey : Colour::White);
	const UIColour textShadowColour(Colour(0.f, 0.f, 0.12f));
	const Vector2 textShadowOffset(2.f, -2.f);

	SetBounds(UIBounds(0.f, y, 1.f, UICoord(0.f, h)));

	SharedPointer<const Font> font = instance.engine.pFontManager->Get("arial", instance.engine.context);
	SharedPointer<const Material> btnMat = instance.engine.pMaterialManager->Get("uibutton1", instance.engine.context);

	_label.SetFont(font).SetColour(textColour).SetShadowColour(textShadowColour).SetShadowOffset(textShadowOffset).SetText(Text(_property.GetName()));
	_label.SetBounds(UIBounds(0.f, 0.f, .5f, 1.f));

	float tbWidthOffset = 0.f;

	if (!readOnly && (property.GetFlags() & PropertyFlags::MATERIAL || property.GetFlags() & PropertyFlags::MODEL))
	{
		tbWidthOffset = -30.f;

		UITextButton* browse = new UITextButton(this);
		browse->SetBounds(UIBounds(UICoord(1.f, -30.f), 0.f, UICoord(0.f, 30.f), 1.f));
		browse->SetText(Text("...")).SetFont(font)
			.SetTextColour(textColour).SetTextShadowColour(textShadowColour).SetTextShadowOffset(textShadowOffset)
			.SetMaterial(btnMat).SetBorderSize(borderSize)
			.SetColour(buttonColourInactive).SetColourHover(buttonColourHover).SetColourHold(buttonColourHold);

		browse->onPressed += Function<void, UIButton&>(&UIPropertyManipulator::_OnPressed, *this);
	}
	else if (property.GetFlags() & PropertyFlags::DIRECTION || property.GetFlags() & PropertyFlags::CLASSID || comboText.GetSize())
	{
		UIComboBox* comboBox = new UIComboBox(this);
		comboBox->SetBounds(UIBounds(.5f, 0.f, .5f, 1.f));
		comboBox->SetReadOnly(readOnly).SetListFont(font).SetListMaterial(btnMat).SetListBorderSize(borderSize).SetListColour(buttonColourInactive)
			.SetListTextColour(textColour).SetListTextShadowColour(textShadowColour).SetListTextShadowOffset(textShadowOffset).SetListItemHeight(h)
			.SetText(Text(GetPropertyString(_property, _object, _editorInstance.engine))).SetFont(font).SetTextColour(textColour).SetTextShadowColour(textShadowColour).SetTextShadowOffset(textShadowOffset)
			.SetMaterial(btnMat).SetBorderSize(borderSize).SetColour(buttonColourInactive).SetColourHover(buttonColourHover).SetColourHold(buttonColourHold);

		if (!readOnly)
		{
			comboBox->onSelectionChanged += Function(&UIPropertyManipulator::_OnComboboxStringChanged, *this);

			if (comboText.GetSize())
			{
				for (const Text& txt : comboText)
					comboBox->Add(txt);
			}
			else if (property.GetFlags() & PropertyFlags::DIRECTION)
			{
				comboBox->Add(instance.engine.pTextProvider->Get("dir_north"));
				comboBox->Add(instance.engine.pTextProvider->Get("dir_east"));
				comboBox->Add(instance.engine.pTextProvider->Get("dir_south"));
				comboBox->Add(instance.engine.pTextProvider->Get("dir_west"));
			}
			else
			{
				const List<ObjectType>& types = instance.engine.objectTypes.GetTypes();
				for (const ObjectType& type : types)
					comboBox->Add(Text(type.GetName()));
			}
		}

		return; //We don't want a textbox, seeya
	}

	if (dynamic_cast<VariableProperty<bool>*>(&_property))
	{
		tbWidthOffset = -h;

		UICheckbox* check = new UICheckbox(this);
		check->SetBounds(UIBounds(UICoord(1.f, -h), 0.f, UICoord(0.f, h), UICoord(0.f, h)));
		check->SetState(dynamic_cast<VariableProperty<bool>&>(_property).Get(_object, _editorInstance.engine.context)).SetReadOnly(readOnly).SetTextureTrue(instance.engine.pTextureManager->Get("ui/checkmark", instance.engine.context)).
			SetMaterial(btnMat).SetBorderSize(borderSize).SetColourFalse(buttonColourInactive).SetColourTrue(buttonColourInactive).SetColourHover(buttonColourHover).SetColourHold(buttonColourHold);
		
		if (!readOnly)
			check->onStateChanged += Function(&UIPropertyManipulator::_OnStateChanged, *this);
	}

	_textbox = new UITextbox(this);
	_textbox->SetBounds(UIBounds(.5f, 0.f, UICoord(.5f, tbWidthOffset), 1.f));
	_textbox->SetReadOnly(readOnly).SetMaterial(btnMat).SetBorderSize(borderSize)
		.SetFont(font).SetTextColour(textColour).SetTextShadowColour(textShadowColour).SetTextShadowOffset(textShadowOffset).SetTextAlignment(ETextAlignment::LEFT)
		.SetText(Text(GetPropertyString(_property, _object, _editorInstance.engine))).SetColour(textboxColour);

	if (!readOnly)
		_textbox->onStringChanged += Function(&UIPropertyManipulator::_OnTextboxStringChanged, *this);
}

void UIPropertyManipulator::Refresh()
{
	_refreshing = true;

	if (_textbox)
	{
		_textbox->SetText(Text(GetPropertyString(_property, _object, _editorInstance.engine)));
	}

	if (_checkbox)
	{
		VariableProperty<bool>* boolProperty = dynamic_cast<VariableProperty<bool>*>(&_property);
		if (boolProperty)
			_checkbox->SetState(boolProperty->Get(_object, _editorInstance.engine.context));
	}

	if (_combobox)
	{
		_combobox->SetText(Text(GetPropertyString(_property, _object, _editorInstance.engine)));
	}

	_refreshing = false;
}
