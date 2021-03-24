#pragma once
#include <ELGraphics/RenderQueue.hpp>
#include <ELMaths/Frustum.hpp>
#include <ELMaths/Projection.hpp>
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

	Text _textPerspective;
	Text _textIsometric;
	Text _textOrthoX;
	Text _textOrthoY;
	Text _textOrthoZ;

public:
	EAxis gridAxis = EAxis::Y;

	Projection cameraProjection;
	Transform cameraTransform;
	Frustum cameraFrustum;

	RenderQueue renderQueue;
	
	UIContainer ui;
	UIRect bg;

	Viewport();
	~Viewport();

	void Initialise(const TextProvider& tp);

	void Resize(const Vector2T<uint16>& dimensions);

	ECameraType GetCameraType() const { return _cameraType; }
	void SetCameraType(ECameraType);

	void SetFont(const SharedPointer<const Font>&);

	void BindFramebuffer() { _idMap.Bind(); }
	bool SampleFramebuffer(uint32 x, uint32 y, uint32 colour[4]) { return _idMap.SampleIntegers(x, y, colour); }
	GLuint GetFramebufferTexGL() const { return _idMap.GetColourTexGL(); }
};
