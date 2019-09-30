#pragma once
#include "GameObject.hpp"
#include "Collision.hpp"
#include "Types.hpp"

enum class ProjectionType
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class ObjCamera : public GameObject
{
private:
	static const ObjCamera* _currentCamera;

	Mat4 _projection;

	ProjectionType _type;

	float _fov;		//Used for perspective
	float _scale;	//Pixels per unit in ortho mode
	float _near;
	float _far;
	Vector<uint16, 2> _viewport;

	void UpdateProjectionMatrix();
public:
	GAMEOBJECT_FUNCS(ObjCamera, ObjectIDS::CAMERA)

	ObjCamera() : _type(ProjectionType::PERSPECTIVE), _fov(90.f), _scale(1.f), _near(.001f), _far(100.f) { }
	~ObjCamera() {}

	void Use();
	inline static const ObjCamera* Current() { return _currentCamera; }

	inline void SetProjectionType(ProjectionType type)	{ _type = type; UpdateProjectionMatrix(); }
	inline void SetFOV(float fieldOfView)				{ _fov = fieldOfView; UpdateProjectionMatrix(); }
	inline void SetScale(float scale)					{ _scale = scale; UpdateProjectionMatrix(); }
	inline void SetZBounds(float n, float f)			{ _near = n; _far = f; UpdateProjectionMatrix(); }
	inline void SetViewport(uint16 width, uint16 height){ _viewport[0] = width; _viewport[1] = height;	UpdateProjectionMatrix(); }

	inline ProjectionType GetProjectionType() const		{ return _type; }
	inline float GetFOV() const							{ return _fov; }
	inline float GetFOVHorizontal() const				{ return _fov * (float)_viewport[0] / (float)_viewport[1]; }
	inline float GetScale() const						{ return _scale; }
	inline const Vector<uint16, 2>& GetViewport() const	{ return _viewport; }
	inline float GetNear() const						{ return _near; }
	inline float GetFar() const							{ return _far; }

	inline const Mat4& GetProjectionMatrix() const		{ return _projection; }

	bool FrustumOverlaps(const Bounds &b) const;

	//coords are between -.5f(top-left) and .5f(bottom-right)
	Ray ScreenCoordsToRay(const Vector2 &coords) const;

	Vector2 GetZPlaneDimensions() const;

	//todo : implement this xd
	//virtual const PropertyCollection& GetProperties() override;

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings) override;
};
