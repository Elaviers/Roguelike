#pragma once
#include <Engine/EntCamera.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELUI/ComboBox.hpp>
#include <ELUI/Container.hpp>
#include <ELUI/Rect.hpp>

class Viewport
{
public:
	enum class ECameraType
	{
		PERSPECTIVE,
		ISOMETRIC,
		ORTHO_X,
		ORTHO_Y,
		ORTHO_Z
	};

protected:
	UIComboBox _comboBox;

	ECameraType _cameraType;

	void _OnComboBoxItemSelected(UIComboBox&);

public:
	Axes::EAxis gridAxis = Axes::EAxis::Y;

	EntCamera camera;

	RenderQueue renderQueue;
	
	UIContainer ui;
	UIRect bg;

	Viewport();

	ECameraType GetCameraType() const { return _cameraType; }
	void SetCameraType(ECameraType);

	void SetFont(const SharedPointer<const Font>&);
};
