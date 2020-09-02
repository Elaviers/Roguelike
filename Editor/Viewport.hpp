#pragma once
#include <Engine/EntCamera.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELUI/ComboBox.hpp>
#include <ELUI/Container.hpp>
#include <ELUI/Rect.hpp>

class Viewport
{
	UIComboBox _comboBox;

	void _OnComboBoxItemSelected(UIComboBox&);

public:
	enum class ECameraType
	{
		PERSPECTIVE,
		ISOMETRIC,
		ORTHO_X,
		ORTHO_Y,
		ORTHO_Z
	};

	EDirection gridPlane = EDirection::UP;

	EntCamera camera;

	RenderQueue renderQueue;
	
	UIContainer ui;
	UIRect bg;

	Viewport();

	void SetCameraType(ECameraType);

	void SetFont(const SharedPointer<const Font>&);
};
