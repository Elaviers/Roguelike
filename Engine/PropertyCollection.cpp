#include "PropertyCollection.hpp"

String PropertyCollection::HandleCommand(void* obj, const String& command) const
{
	Buffer<String> tokens = command.Split(" ");

	if (tokens.GetSize() > 0)
	{
		String name = tokens[0].ToLower();

		Property* property = Find(name);

		if (property)
		{
			if (property->GetType() == PropertyType::FUNCTION)
			{
				tokens.RemoveIndex(0);

				auto func = dynamic_cast<const FunctionPropertyBase*>(property);
				if (func)	func->Call(obj, tokens);
				else		((VariableProperty<CommandPtr>*)property)->Get(obj)(tokens);
				return "";
			}

			if (tokens.GetSize() >= 2)
				property->SetAsString(obj, tokens[1]);

			return name + " : " + property->GetAsString(obj);
		}
	}

	return "";
}
