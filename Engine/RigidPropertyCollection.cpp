#include "RigidPropertyCollection.hpp"

String RigidPropertyCollection::HandleCommand(const String& command) const
{
	Buffer<String> tokens = command.Split(" ");

	if (tokens.GetSize() > 0)
	{
		String name = tokens[0].ToLower();

		RigidProperty* property = Find(name);

		if (property)
		{
			if (property->GetType() == PropertyType::FUNCTION)
			{
				tokens.RemoveIndex(0);

				auto func = dynamic_cast<const RigidFunctionPropertyBase*>(property);
				if (func)	func->Call(tokens);
				else		((RigidVariableProperty<CommandPtr>*)property)->Get()(tokens);
				return "";
			}

			if (tokens.GetSize() >= 2)
				property->SetAsString(tokens[1]);

			return name + " : " + property->GetAsString();
		}
	}

	return "";
}
