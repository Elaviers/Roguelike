#include "Viewport.hpp"
#include <ELLib/ELCore/TextProvider.hpp>

Viewport::Viewport() : _cameraType(ECameraType::PERSPECTIVE)
{
	_comboBox.SetParent(&ui);
	bg.SetParent(&ui);

	_comboBox.SetBounds(UIBounds(UICoord(1.f, -128.f), UICoord(1.f, -16.f), UICoord(0.f, 110.f), UICoord(0.f, 16.f)));
	_comboBox.SetTextAlignment(ETextAlignment::RIGHT).SetTextMargin(0.f).SetColour(Colour::Invisible).SetColourHover(Colour(0.f, 0.f, 1.f, 0.25f)).SetColourHold(Colour(0.f, 0.f, 1.f, 0.5f)).SetBorderSize(0.f);
	_comboBox.SetListColour(Colour::Invisible).SetListTextAlignment(ETextAlignment::RIGHT).SetListTextMargin(0.f).SetListItemHeight(16.f);
	
	_comboBox.onSelectionChanged += FunctionPointer(this, &Viewport::_OnComboBoxItemSelected);
}

Viewport::~Viewport()
{
	_idMap.Delete();
}

void Viewport::Initialise(const TextProvider& tp)
{
	_textPerspective = tp.Get("vp_camera_perspective");
	_textIsometric = tp.Get("vp_camera_isometric");
	_textOrthoX = tp.Get("vp_camera_ortho_x");
	_textOrthoY = tp.Get("vp_camera_ortho_y");
	_textOrthoZ = tp.Get("vp_camera_ortho_z");

	_comboBox.Add(_textPerspective);
	_comboBox.Add(_textIsometric);
	_comboBox.Add(_textOrthoX);
	_comboBox.Add(_textOrthoY);
	_comboBox.Add(_textOrthoZ);
}

void Viewport::_OnComboBoxItemSelected(UIComboBox& comboBox)
{
	const Text& item = comboBox.GetText();
	
	if (item == _textPerspective)
		SetCameraType(Viewport::ECameraType::PERSPECTIVE);
	else if (item == _textIsometric)
		SetCameraType(Viewport::ECameraType::ISOMETRIC);
	else if (item == _textOrthoX)
		SetCameraType(Viewport::ECameraType::ORTHO_X);
	else if (item == _textOrthoY)
		SetCameraType(Viewport::ECameraType::ORTHO_Y);
	else if (item == _textOrthoZ)
		SetCameraType(Viewport::ECameraType::ORTHO_Z);
}

void Viewport::Resize(const Vector2T<uint16>& dimensions)
{
	camera.GetProjection().SetDimensions(dimensions);

	_idMap.CreateColourDepth(dimensions.x, dimensions.y);
}

void Viewport::SetCameraType(Viewport::ECameraType type)
{
	_cameraType = type;

	switch (type)
	{
	case Viewport::ECameraType::PERSPECTIVE:
		_comboBox.SetText(_textPerspective);
		gridAxis = EAxis::Y;
		camera.GetProjection().SetType(EProjectionType::PERSPECTIVE);
		camera.GetProjection().SetNearFar(0.001f, 100.f);
		camera.SetRelativePosition(Vector3(-5.f, 5.f, -5.f));
		camera.SetRelativeRotation(Vector3(-45.f, 45.f, 0.f));
		break;

	case Viewport::ECameraType::ISOMETRIC:
		_comboBox.SetText(_textIsometric);
		gridAxis = EAxis::Y;
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetOrthographicScale(8.f);
		camera.GetProjection().SetNearFar(0.f, 20000.f);
		camera.SetRelativeRotation(Vector3(-Maths::ArcTangentDegrees(1.f / Maths::SQRT2_F), 45.f, 0.f));
		camera.SetRelativePosition(camera.GetRelativeTransform().GetForwardVector() * -10000.f);
		break;

	case Viewport::ECameraType::ORTHO_X:
		_comboBox.SetText(_textOrthoX);
		gridAxis = EAxis::X;
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetOrthographicScale(8.f);
		camera.GetProjection().SetNearFar(0.f, 20000.f);
		camera.SetRelativeRotation(Vector3(0.f, -90.f, 0.f));
		camera.SetRelativePosition(Vector3(10000.f, 0.f, 0.f));
		break;

	case Viewport::ECameraType::ORTHO_Y:
		_comboBox.SetText(_textOrthoY);
		gridAxis = EAxis::Y;
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetOrthographicScale(8.f);
		camera.GetProjection().SetNearFar(0.f, 20000.f);
		camera.SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
		camera.SetRelativePosition(Vector3(0.f, 10000.f, 0.f));
		break;

	case Viewport::ECameraType::ORTHO_Z:
		_comboBox.SetText(_textOrthoZ);
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
