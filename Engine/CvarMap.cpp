#include "CvarMap.hpp"
#include "Buffer.hpp"
#include "FunctionPointer.hpp"

String CvarMap::HandleCommand(const String& command)
{
	Buffer<String> tokens = command.Split(" ");

	if (tokens.GetSize() > 0)
	{
		String command = tokens[0].ToLower();

		Cvar* cvar = FindRaw(command);

		if (cvar)
		{
			if (cvar->GetType() == CvarType::FUNCTION)
			{
				tokens.RemoveIndex(0);
				((TypedCvar<CommandPtr>*)cvar)->Call(tokens);
				return "";
			}

			if (tokens.GetSize() >= 2)
				cvar->SetByString(tokens[1]);

			return command + " : " + cvar->GetAsString();
		}
	}

	return "";
}
