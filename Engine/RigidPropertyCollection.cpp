#include "RigidPropertyCollection.hpp"
#include "Colour.hpp"
#include "Console.hpp"
#include "Engine.hpp"

void RigidPropertyCollection::HandleCommand(const Buffer<String>& tokens) const
{
	if (tokens.GetSize() > 0)
	{
		String name = tokens[0].ToLower();

		RigidProperty* property = Find(name);

		if (property)
		{
			if (property->GetType() == PropertyType::FUNCTION)
			{
				Buffer<String> newTokens(&tokens[1], tokens.GetSize() - 1);

				((RigidVariableProperty<CommandPtr>*)property)->Get()(newTokens);
				return;
			}

			if (tokens.GetSize() >= 2)
				property->SetAsString(tokens[1]);

			Engine::Instance().pConsole->Print(CSTR(name, " : ", property->GetAsString()));
		}
	}
}
