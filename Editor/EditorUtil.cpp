#include "EditorUtil.hpp"
#include "MouseData.hpp"
#include <Engine/World.hpp>
#include <Engine/WorldObject.hpp>
#include <Engine/imgui/imgui.h>

void EditorUtil::CalculatePointsFromMouseData(const MouseData &mouseData, Vector2 &out_p1, Vector2 &out_p2)
{
	if (mouseData.unitX_rounded <= mouseData.heldUnitX_rounded)
	{
		out_p1[0] = (float)(mouseData.heldUnitX_rounded + 1);
		out_p2[0] = (float)mouseData.unitX_rounded;
	}
	else
	{
		out_p1[0] = (float)mouseData.heldUnitX_rounded;
		out_p2[0] = (float)(mouseData.unitX_rounded + 1);
	}

	if (mouseData.unitY_rounded <= mouseData.heldUnitY_rounded)
	{
		out_p1[1] = (float)(mouseData.heldUnitY_rounded + 1);
		out_p2[1] = (float)mouseData.unitY_rounded;
	}
	else
	{
		out_p1[1] = (float)mouseData.heldUnitY_rounded;
		out_p2[1] = (float)(mouseData.unitY_rounded + 1);
	}
}

bool ObjectIMGUI(WorldObject* object, Buffer<WorldObject*>& selection)
{
	ImGuiTreeNodeFlags flags = 0;

	if (object->GetChildren().GetSize() <= 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (selection.IndexOfFirst(object) >= 0)
		flags |= ImGuiTreeNodeFlags_Selected;

	bool hasName = object->GetName().ToString().GetLength() > 0;

	String fmtStr = hasName ? "[%d][%s] \"%s\"" : "[%d][%s]";
	const char* fmt = fmtStr.GetData();

	bool expanded = ImGui::TreeNodeEx(
		(void*)object->GetUID(), flags,
		fmt, object->GetUID(), object->GetClassString(), object->GetName().ToString().GetData());
	
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("WorldObject", &object, sizeof(WorldObject*));
		ImGui::Text(fmt, object->GetUID(), object->GetClassString(), object->GetName().ToString().GetData());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("WorldObject"))
		{
			WorldObject* newChild = *(WorldObject**)payload->Data;

			if (object != newChild && !object->IsChildOf(*newChild))
			{
				newChild->SetParent(object, true);
			}
		}

		ImGui::EndDragDropTarget();
	}

	bool result = false;
	if (ImGui::IsItemClicked())
	{
		result = true;

		if (ImGui::GetIO().KeyCtrl)
		{
			if ((flags & ImGuiTreeNodeFlags_Selected) == 0)
				selection.Add(object);
		}
		else
			selection = { object };
	}

	if (expanded)
	{
		for (WorldObject* child : object->GetChildren())
		{
			if (ObjectIMGUI(child, selection))
				result = true;
		}

		ImGui::TreePop();
	}

	return result;
}

bool EditorUtil::WorldIMGUI(World& world, Buffer<WorldObject*>& selection)
{
	bool result = false;

	if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("WorldObject"))
			{
				WorldObject* newChild = *(WorldObject**)payload->Data;
				newChild->SetParent(nullptr, true);
			}

			ImGui::EndDragDropTarget();
		}

		for (WorldObject* object : world.GetObjects())
		{
			if (object->GetParent() == nullptr)
			{
				if (ObjectIMGUI(object, selection))
					result = true;
			}
		}
	}

	return result;
}
