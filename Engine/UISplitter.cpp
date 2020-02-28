#include "UISplitter.hpp"
#include "GLProgram.hpp"
#include "UIContainer.hpp"
#include "ModelManager.hpp"
#include "TextureManager.hpp"

void UISplitter::_OnBoundsChanged()
{
	_transform.SetPosition(Vector3(_absoluteBounds.x + _absoluteBounds.w / 2.f, _absoluteBounds.y + _absoluteBounds.h / 2.f, _z));
	
	if (_isHorizontal)
		_transform.SetScale(Vector3(_absoluteBounds.w, _absoluteBounds.h, 0.f));
	else
		_transform.SetScale(Vector3(_absoluteBounds.h, _absoluteBounds.w, 0.f));

	UIContainer* parent = dynamic_cast<UIContainer*>(_parent);

	if (_parent)
	{
		if (_isHorizontal)
		{
			float y = Maths::Clamp(_relativeBounds.y, 0.f, 1.f);
			float relH = _absoluteBounds.h / parent->GetAbsoluteBounds().h / 2.f;

			for (size_t i = 0; i < parent->GetChildren().GetSize(); ++i)
			{
				UIElement* child = parent->GetChildren()[i];
				if (child && dynamic_cast<UISplitter*>(child) == nullptr)
				{
					RelativeBounds newBounds = child->GetRelativeBounds();

					if (_after.Contains(child))
					{
						newBounds.y = y + relH;
						newBounds.h = 1.f - y - relH;
					}
					else
					{
						newBounds.y = 0.f;
						newBounds.h = y - relH;
					}

					child->SetBounds(newBounds);
					onDragged(*this);
				}
			}
		}
		else
		{
			float x = Maths::Clamp(_relativeBounds.x, 0.f, 1.f);
			float relW = _absoluteBounds.w / parent->GetAbsoluteBounds().w / 2.f;

			for (size_t i = 0; i < parent->GetChildren().GetSize(); ++i)
			{
				UIElement* child = parent->GetChildren()[i];
				if (child && dynamic_cast<UISplitter*>(child) == nullptr)
				{
					RelativeBounds newBounds = child->GetRelativeBounds();

					if (_after.Contains(child))
					{
						newBounds.x = x + relW;
						newBounds.w = 1.f - x - relW;
					}
					else
					{
						newBounds.x = 0.f;
						newBounds.w = x - relW;
					}

					child->SetBounds(newBounds);
					onDragged(*this);
				}
			}
		}
	}
}

void UISplitter::Render() const
{
	if (_texture)
		_texture->Bind(0);
	else
		Engine::Instance().pTextureManager->White()->Bind(0);

	_colour.ApplyToShader();
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, _transform.GetTransformationMatrix());
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, Vector2());
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2((_isHorizontal ? _absoluteBounds.w : _absoluteBounds.h) / _textureTileSize, 1.f));
	Engine::Instance().pModelManager->Plane()->Render();
	GLProgram::Current().SetBool(DefaultUniformVars::boolBlend, false);
}

void UISplitter::OnMouseMove(float mouseX, float mouseY)
{
	_hover = OverlapsPoint(mouseX, mouseY);

	if (_drag)
	{
		if (_isHorizontal)
		{
			_relativeBounds.y = (mouseY - _parent->GetAbsoluteBounds().y) / _parent->GetAbsoluteBounds().h;
			UpdateAbsoluteBounds();
		}
		else
		{
			_relativeBounds.x = (mouseX - _parent->GetAbsoluteBounds().x) / _parent->GetAbsoluteBounds().w;
			UpdateAbsoluteBounds();
		}
	}
}

void UISplitter::OnMouseUp()
{
	_drag = false;
}

void UISplitter::OnMouseDown()
{
	if (_hover)
		_drag = true;
}
