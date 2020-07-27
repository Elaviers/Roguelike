#include "RigidPropertyCollection.hpp"

String RigidPropertyCollection::HandleCommand(const Buffer<String>& tokens, const Context& ctx) const
{
	if (tokens.GetSize() > 0)
	{
		RigidProperty* property = Find(tokens[0].ToLower());

		if (property)
		{
			Buffer<String> args(&tokens[1], tokens.GetSize() - 1);
			return property->HandleCommand(args, ctx);
		}
	}

	return "";
}
