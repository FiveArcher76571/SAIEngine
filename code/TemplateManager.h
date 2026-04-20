// Template manager header file (TemplateManager.h)
// Holds TemplateManager class declaration

#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include "Actor.h"
#include "ComponentManager.h"
#include "JSONReader.h"
#include "LuaManager.h"

#include "rapidjson/document.h"
#include "lua.hpp"
#include "LuaBridge.h"

class TemplateManager {

	// Container of all templates
	static inline std::unordered_map<std::string, Actor> template_list;

public:

	// TemplateManager initializer
	// Reads in all templates from resources/actor_templates
	void initialize();

	// Check if a template exists given its name
	// If it does, assign the given actor to its values and return true
	// If not, return false
	bool check_and_assign_template(const std::string &template_name, Actor &actor);

};

