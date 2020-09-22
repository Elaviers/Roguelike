#include "Viewport.hpp"

Viewport::Viewport() : _cameraType(ECameraType::PERSPECTIVE)
{
	_comboBox.SetParent(&ui);
	bg.SetParent(&ui);

	_comboBox.SetBounds(UICoord(1.f, -128.f), UICoord(1.f, -16.f), UICoord(0.f, 110.f), UICoord(0.f, 16.f));
	_comboBox.SetTextAlignment(ETextAlignment::RIGHT).SetTextMargin(0.f).SetColour(Colour::Invisible).SetColourHover(Colour(0.f, 0.f, 1.f, 0.25f)).SetColourHold(Colour(0.f, 0.f, 1.f, 0.5f)).SetBorderSize(0.f);
	_comboBox.SetListColour(Colour::Invisible).SetListTextAlignment(ETextAlignment::RIGHT).SetListTextMargin(0.f).SetListItemHeight(16.f);
	_comboBox.Add("Perspective");
	_comboBox.Add("Isometric");
	_comboBox.Add("X");
	_comboBox.Add("Y");
	_comboBox.Add("Z");
	_comboBox.onStringChanged += FunctionPointer(this, &Viewport::_OnComboBoxItemSelected);
}

void Viewport::_OnComboBoxItemSelected(UIComboBox& comboBox)
{
	String string = comboBox.GetString().ToLower();
	if (string == "perspective")
		SetCameraType(Viewport::ECameraType::PERSPECTIVE);
	else if (string == "isometric")
		SetCameraType(Viewport::ECameraType::ISOMETRIC);
	else if (string == "x")
		SetCameraType(Viewport::ECameraType::ORTHO_X);
	else if (string == "y")
		SetCameraType(Viewport::ECameraType::ORTHO_Y);
	else if (string == "z")
		SetCameraType(Viewport::ECameraType::ORTHO_Z);
}

void Viewport::SetCameraType(Viewport::ECameraType type)
{
	_cameraType = type;

	switch (type)
	{
	case Viewport::ECameraType::PERSPECTIVE:
		_comboBox.SetString("Perspective");
		gridAxis = EAxis::Y;
		camera.GetProjection().SetType(EProjectionType::PERSPECTIVE);
		camera.GetProjection().SetNearFar(0.001f, 100.f);
		camera.SetRelativePosition(Vector3(-5.f, 5.f, -5.f));
		camera.SetRelativeRotation(Vector3(-45.f, 45.f, 0.f));
		break;

	case Viewport::ECameraType::ISOMETRIC:
		_comboBox.SetString("Isometric");
		gridAxis = EAxis::Y;
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetOrthographicScale(8.f);
		camera.GetProjection().SetNearFar(0.f, 20000.f);
		camera.SetRelativeRotation(Vector3(-Maths::ArcTangentDegrees(1.f / Maths::SQRT2_F), 45.f, 0.f));
		camera.SetRelativePosition(camera.GetRelativeTransform().GetForwardVector() * -10000.f);
		break;

	case Viewport::ECameraType::ORTHO_X:
		_comboBox.SetString("X");
		gridAxis = EAxis::X;
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetOrthographicScale(8.f);
		camera.GetProjection().SetNearFar(0.f, 20000.f);
		camera.SetRelativeRotation(Vector3(0.f, -90.f, 0.f));
		camera.SetRelativePosition(Vector3(10000.f, 0.f, 0.f));
		break;

	case Viewport::ECameraType::ORTHO_Y:
		_comboBox.SetString("Y");
		gridAxis = EAxis::Y;
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetOrthographicScale(8.f);
		camera.GetProjection().SetNearFar(0.f, 20000.f);
		camera.SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
		camera.SetRelativePosition(Vector3(0.f, 10000.f, 0.f));
		break;

	case Viewport::ECameraType::ORTHO_Z:
		_comboBox.SetString("Z");
		gridAxis = EAxis::Z;
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetOrthographicScale(8.f);
		camera.GetProjection().SetNearFar(0.f, 20000.f);
		camera.SetRelativeRotation(Vector3(0.f, 0.f, 0.f));
		camera.SetRelativePosition(Vector3(0.f, 0.f, -10000.f));
		break;
			
	}
}

void Viewport::SetFont(const SharedPointer<const Font>& font)
{
	_comboBox.SetFont(font);
	_comboBox.SetListFont(font);
}
