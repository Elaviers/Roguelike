#include "ToolBrush3D.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include "UIPropertyManipulator.hpp"
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& ToolBrush3D::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE(
		properties.Add(
		"Material",
		MemberGetter<ToolBrush3D, const String&>(&ToolBrush3D::_P_GetMaterialName),
		MemberSetter<ToolBrush3D, String>(&ToolBrush3D::_P_SetMaterialName),
		0,
		PropertyFlags::MATERIAL)
	);

	return properties;
}

void ToolBrush3D::_P_SetMaterialName(const String& materialName)
{
	_materialName = materialName;

	if (_object)
		_object->SetMaterial(materialName, _owner.engine.context);
}

void ToolBrush3D::Initialise()
{
	_materialName = "alt";
}

void ToolBrush3D::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	_object = _owner.WorldRef().CreateObject<OBrush3D>();
	_object->SetRelativeScale(Vector3());
	_object->SetMaterial(_materialName, _owner.engine.context);
	
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _object->GetProperties(), _object, properties);
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolBrush3D::Deactivate()
{
	_object->Destroy();
}

void ToolBrush3D::Cancel()
{
	_placing = false;
}

void ToolBrush3D::MouseMove(MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->cameraProjection.GetType() == EProjectionType::ORTHOGRAPHIC)
	{
		EAxis fwdAxis = mouseData.viewport->gridAxis;
		int fwdElement = (int)fwdAxis;
		int rightElement = (int)Axes::GetHorizontalAxis(fwdAxis);
		int upElement = (int)Axes::GetVerticalAxis(fwdAxis);

		if (mouseData.isLeftDown)
		{
			Vector2 p1, p2;
			EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

			Vector3 p13d = _object->GetPoint1();
			Vector3 p23d = _object->GetPoint2();

			p13d[rightElement] = (float)p1[0];
			p13d[upElement] = (float)p1[1];
			p23d[rightElement] = (float)p2[0];
			p23d[upElement] = (float)p2[1];

			_object->SetPoint1(p13d);
			_object->SetPoint2(p23d);
			_owner.RefreshProperties();
		}
		else if (!_placing)
		{
			Vector3 p1;
			p1[rightElement] = (float)mouseData.unitX_rounded;
			p1[upElement] = (float)mouseData.unitY_rounded;
			p1[fwdElement] = -100;

			Vector3 p2;
			p2[rightElement] = (float)(mouseData.unitX_rounded + 1);
			p2[upElement] = (float)(mouseData.unitY_rounded + 1);
			p2[fwdElement] = 100;

			_object->SetPoint1(p1);
			_object->SetPoint2(p2);
			_owner.RefreshProperties();
		}
	}
}

void ToolBrush3D::MouseDown(MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->cameraProjection.GetType() == EProjectionType::ORTHOGRAPHIC) _placing = true;
}

void ToolBrush3D::KeySubmit()
{
	_placing = false;

	_object = _owner.WorldRef().CreateObject<OBrush3D>();
}

void ToolBrush3D::Render(RenderQueue& q) const
{
	RenderEntry& box = q.CreateEntry(ERenderChannels::UNLIT);
	box.AddSetLineWidth(2.f);
	box.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	box.AddSetColour(Colour::Green);

	Vector3 min, max;
	_object->GetVolume()->Bounds_AABB(_object->GetAbsoluteTransform(), min, max);
	box.AddBox(min, max);

	_object->Render(q);
}
