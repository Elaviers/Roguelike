#include "PropertyCollection.hpp"

String PropertyCollection::HandleCommand(void* obj, const Buffer<String>& tokens) const
{
	if (tokens.GetSize() > 0)
	{
		String name = tokens[0].ToLower();

		Property* property = Find(name);

		if (property)
		{
			if (property->GetType() == EPropertyType::FUNCTION)
			{
				Buffer<String> newTokens(&tokens[1], tokens.GetSize() - 1);
				
				auto func = dynamic_cast<const FunctionPropertyBase*>(property);
				if (func)	func->Call(obj, newTokens);
				else		((VariableProperty<CommandPtr>*)property)->Get(obj)(newTokens);

				return "";
			}

			if (tokens.GetSize() >= 2)
				property->SetAsString(obj, tokens[1]);

			return name + " : " + property->GetAsString(obj) + '\n';
		}
	}

	return "";
}
