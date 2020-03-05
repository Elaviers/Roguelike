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

	if (parent)
	{
		if (_isHorizontal)
		{
			float relY = Maths::Clamp(_absoluteBounds.y / parent->GetAbsoluteBounds().h, 0.f, 1.f);

			for (size_t i = 0; i < parent->GetChildren().GetSize(); ++i)
			{
				UIElement* child = parent->GetChildren()[i];
				if (child && dynamic_cast<UISplitter*>(child) == nullptr)
				{
					UIBounds newBounds = child->GetBounds();

					if (_after.Contains(child))
					{
						float relH = _absoluteBounds.h / parent->GetAbsoluteBounds().h;
						newBounds.y = relY + relH;
						newBounds.h = 1.f - relY - relH;
					}
					else
					{
						newBounds.y = 0.f;
						newBounds.h = relY;
					}

					child->SetBounds(newBounds);
					onDragged(*this);
				}
			}
		}
		else
		{
			float relX = Maths::Clamp(_absoluteBounds.x / parent->GetAbsoluteBounds().w, 0.f, 1.f);

			for (size_t i = 0; i < parent->GetChildren().GetSize(); ++i)
			{
				UIElement* child = parent->GetChildren()[i];
				if (child && dynamic_cast<UISplitter*>(child) == nullptr)
				{
					UIBounds newBounds = child->GetBounds();

					if (_after.Contains(child))
					{
						float relW = _absoluteBounds.w / parent->GetAbsoluteBounds().w;
						newBounds.x = relX + relW;
						newBounds.w = 1.f - relX - relW;
					}
					else
					{
						newBounds.x = 0.f;
						newBounds.w = relX;
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
	UIRect::OnMouseMove(mouseX, mouseY);

	if (_drag)
	{
		if (_isHorizontal)
		{
			if (_useAbsolute)
			{
				_bounds.y.absolute = mouseY - _bounds.y.relative * _parent->GetAbsoluteBounds().h - _absoluteBounds.h / 2.f;
				_bounds.y.absolute = Maths::Clamp(_bounds.y.absolute, _min, _max);
			}
			else
			{
				_bounds.y.relative = (mouseY - _parent->GetAbsoluteBounds().y - _bounds.y.absolute - _absoluteBounds.h / 2.f) / _parent->GetAbsoluteBounds().h;
				_bounds.y.relative = Maths::Clamp(_bounds.y.relative, _min, _max);
			}

			UpdateAbsoluteBounds();
		}
		else
		{
			if (_useAbsolute)
			{
				_bounds.x.absolute = mouseX - _bounds.x.relative * _parent->GetAbsoluteBounds().w - _absoluteBounds.w / 2.f;
				_bounds.x.absolute = Maths::Clamp(_bounds.x.absolute, _min, _max);
			}
			else
			{
				_bounds.x.relative = (mouseX - _parent->GetAbsoluteBounds().x - _bounds.x.absolute - _absoluteBounds.w / 2.f) / _parent->GetAbsoluteBounds().w;
				_bounds.x.relative = Maths::Clamp(_bounds.x.relative, _min, _max);
			}

			UpdateAbsoluteBounds();
		}
	}
}

bool UISplitter::OnMouseUp()
{
	if (_drag)
	{
		_drag = false;
		return true;
	}

	return false;
}

bool UISplitter::OnMouseDown()
{
	if (_hover)
	{
		_drag = true;
		return true;
	}

	return false;
}
