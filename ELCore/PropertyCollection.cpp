#include "PropertyCollection.hpp"

String PropertyCollection::HandleCommand(void* obj, const Buffer<String>& tokens, const Context& ctx) const
{
	if (tokens.GetSize() > 0)
	{
		Property* property = Find(tokens[0].ToLower());

		if (property)
		{
			Buffer<String> args(&tokens[1], tokens.GetSize() - 1);
			return property->HandleCommand(obj, args, ctx);
		}
	}

	return "";
}
