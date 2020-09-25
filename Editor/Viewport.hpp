#pragma once
#include <Engine/EntCamera.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELSys/GLFramebuffer.hpp>
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
	GLFramebuffer _idMap;

	UIComboBox _comboBox;

	ECameraType _cameraType;

	void _OnComboBoxItemSelected(UIComboBox&);

public:
	EAxis gridAxis = EAxis::Y;

	EntCamera camera;

	RenderQueue renderQueue;
	RenderQueue renderQueue2;
	
	UIContainer ui;
	UIRect bg;

	Viewport();
	~Viewport();

	void Resize(const Vector2T<uint16>& dimensions);

	ECameraType GetCameraType() const { return _cameraType; }
	void SetCameraType(ECameraType);

	void SetFont(const SharedPointer<const Font>&);

	void BindFramebuffer() { _idMap.Bind(); }
	Colour SampleFramebuffer(uint32 x, uint32 y) { return _idMap.SampleColour(x, y); }
	GLuint GetFramebufferTexGL() const { return _idMap.GetColourTexGL(); }
};
