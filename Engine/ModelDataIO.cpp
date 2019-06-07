#include "IO.hpp"
#include "BufferIterator.hpp"
#include "Mesh_Skeletal.hpp"
#include "Mesh_Static.hpp"

bool IO::WriteMesh(const char* filename, const Mesh* mesh)
{
	Buffer<byte> data;
	BufferIterator<byte> iterator(data);

	mesh->WriteData(iterator);

	if (IO::WriteFile(filename, data.Data(), data.GetSize()))
		return true;

	return false;
}

Mesh* IO::ReadMesh(const char* filename)
{
	Mesh* mesh = nullptr;

	Buffer<byte> buffer = IO::ReadFile(filename);
	
	switch (buffer[0])
	{
	case DataIDs::MESH_SKELETAL:
		mesh = new Mesh_Skeletal();
		break;
	case DataIDs::MESH_STATIC:
		mesh = new Mesh_Static();
		break;
	}

	if (buffer.GetSize() > 0 && mesh)
	{
		BufferIterator<byte> iterator(buffer);

		mesh->ReadData(iterator);
		mesh->bounds.RecalculateSphereBounds();
	}

	return mesh;
}
