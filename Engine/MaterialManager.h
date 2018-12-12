#pragma once
#include "ResourceManager.h"
#include "Callback.h"
#include "Material.h"

class MaterialManager : public ResourceManager<Material*>
{
private:
	Map<String, Command> _commands;

	void Command_SetMag(const Buffer<String> &args);

public:
	MaterialManager() 
	{
		_commands["setmag"] = Command(this, &MaterialManager::Command_SetMag);
	}

	void HandleCommand(const String &command);

	virtual ~MaterialManager() {}

	const Material* GetMaterial(const String &name);
};
 