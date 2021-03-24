#include "OGeometryCollection.hpp"
#include "Geometry.hpp"
#include "GeometryType.hpp"

OGeometryCollection::OGeometryCollection(World& world) : WorldObject(world), _nextUID(1)
{

}

OGeometryCollection::OGeometryCollection(const OGeometryCollection& other) : WorldObject(other), _nextUID(1)
{

}

OGeometryCollection::~OGeometryCollection()
{
	for (Geometry* g : _geometry)
		delete g;
}

void OGeometryCollection::DeleteGeometry(Geometry* geometry)
{
	if (geometry)
		for (auto it = _geometry.begin(); it.IsValid(); ++it)
			if ((*it)->GetUID() == geometry->GetUID())
			{
				_geometry.Remove(it);
				delete geometry;
				return;
			}
}

void OGeometryCollection::Render(RenderQueue& q) const
{
	for (Geometry* g : _geometry)
		if (g->GetVolume().OverlapsFrustum(Transform::Identity(), _lastViewFrustum))
			g->Render(q);
}

bool OGeometryCollection::IsVisible(const Frustum& view) const
{
	//Yucky const cast
	const_cast<Frustum&>(_lastViewFrustum) = view;
	return true; //Always call render
}

void OGeometryCollection::Read(ByteReader& reader, ObjectIOContext& ioContext)
{
	GeometryRegister* types = ioContext.context.GetPtr<GeometryRegister>();

	uint32 count = reader.Read_uint32();

	for (uint32 i = 0; i < count; ++i)
	{
		EGeometryID id = (EGeometryID)reader.Read_byte();
		const GeometryType* type = types->GetType(id);
		
		if (type)
		{
			Geometry* geometry = type->New(*this);
			geometry->Read(reader, ioContext);
		}
		else
			Debug::Error(CSTR("Invalid geometry ID ", (int)id));
	}
}

void OGeometryCollection::Write(ByteWriter& writer, ObjectIOContext& ioContext) const
{
	writer.Write_uint32(_geometry.GetSize());

	for (Geometry* g : _geometry)
	{
		writer.Write_byte((byte)g->GetTypeID());
		g->Write(writer, ioContext);
	}
}

#include <ELGraphics/RenderQueue.hpp>

void OGeometryCollection::RenderID(RenderQueue& q, uint32 red) const
{
	uint32 colour[4] = { red, 0, 0, 0xFFFFFFFF };
	
	for (Geometry* g : _geometry)
		if (g->GetVolume().OverlapsFrustum(GetAbsoluteTransform(), _lastViewFrustum))
		{
			colour[1] = g->GetUID();
			q.CreateEntry(ERenderChannels::ALL).AddSetUVec4(RCMDSetUVec4::Type::COLOUR, colour);
			g->Render(q);
		}
}

Geometry* OGeometryCollection::DecodeIDMapValue(uint32 red, uint32 green) const
{
	for (Geometry* g : _geometry)
		if (g->GetUID() == green)
			return g;

	return nullptr;
}
