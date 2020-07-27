#include "Asset.hpp"
#include "ByteWriter.hpp"

Buffer<byte> Asset::GetAsData() const
{
	Buffer<byte> data;
	ByteWriter writer(data);
	Write(writer);
	return data;
}
