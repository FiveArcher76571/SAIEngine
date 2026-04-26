// Actor data type definition file (Actor.cpp)
// Defines public Actor functions

#include "Actor.h"

// Initialize Actor data given info from a JSON array/location
void Actor::initialize(const rapidjson::Value &actor_data, const rapidjson::SizeType &i, const int &actor_id_counter) {

	// Create a shorthand for the actor data document
	auto *actor_info = &actor_data;

	// If we've been passed in an array and iterator, set the actor_info pointer to that instead
	if (actor_id_counter != -1) actor_info = &actor_data[i];

	// Go through and populate data from the given document/value...

	// Actor ID
	id = actor_id_counter;

	// Actor name
	if (actor_info->HasMember("name")) name = actor_info->FindMember("name")->value.GetString();

	// Components
	if (actor_info->HasMember("components")) {

		// Get the list of components
		const rapidjson::GenericObject<true, rapidjson::Value> &component_list = actor_info->FindMember("components")->value.GetObject();

		// If the component array is empty, don't do anything...
		if (!component_list.ObjectEmpty()) {

			// Go through all the components listed...
			for (auto comp = component_list.MemberBegin(); comp != component_list.MemberEnd(); comp++) {

				// Get the listed component from the component manager...

				std::shared_ptr<luabridge::LuaRef> comp_base = nullptr;

				// If we're pulling from a template, we can find it in our own component list, and we don't have to look for type per se
				bool template_override = actor_info->HasMember("template") && components.find(comp->name.GetString()) != components.end();

				// If we are...
				if (template_override) {

					// Make the base (to inherit from) that original table instead
					comp_base = components[comp->name.GetString()].ref;

				}

				// Otherwise, pull base from the type (required)
				else comp_base = ComponentManager::get_component(comp->value["type"].GetString());

				// Make an instance of the component
				std::shared_ptr<luabridge::LuaRef> comp_instance = std::make_shared<luabridge::LuaRef>(luabridge::newTable(LuaManager::get_lua_state()));

				// Establish the new instance's inheritance
				ComponentManager::establish_inheritance(*comp_instance, *comp_base);

				// Add the key to the instance
				luabridge::LuaRef &comp_ref = *comp_instance;
				comp_ref["key"] = comp->name.GetString();

				// Add enabled flag to the instance
				comp_ref["enabled"] = true;

				// Get a reference to the base component
				luabridge::LuaRef &base_ref = *comp_base;

				// Now, process any overridden properties...
				for (auto property = comp->value.MemberBegin(); property != comp->value.MemberEnd(); property++) {

					// If we're at the "type" property skip it
					if (property->name.GetString() == "type") continue;

					// Re-assign the property based on its type in the base component...
					if (base_ref[property->name.GetString()].isBool()) comp_ref[property->name.GetString()] = property->value.GetBool();
					else if (base_ref[property->name.GetString()].isNumber()) {

						// If we're passing an integer in (floor is the same as the original number), send an integer
						if (glm::floor(property->value.GetFloat()) == property->value.GetFloat()) comp_ref[property->name.GetString()] = property->value.GetInt();
						else comp_ref[property->name.GetString()] = property->value.GetFloat();

					}
					else if (base_ref[property->name.GetString()].isString()) comp_ref[property->name.GetString()] = property->value.GetString();

				}

				// All done!
				
				// If we're overriding a template, don't store key again, and...
				if (template_override) {

					// Store type name of this component
					std::string type_name = components[comp->name.GetString()].type;

					// Replace the original component with our new one
					components.erase(comp->name.GetString());
					components.insert({ comp->name.GetString(), { type_name, comp_instance}});

					// Move on
					continue;

				}

				// Otherwise, store the component to our list
				components.insert({ comp->name.GetString(),{ comp->value.FindMember("type")->value.GetString(), comp_instance } });

				// Also store the name in the sorted list
				insert_alphabetical(comp->name.GetString());

			}

		}

	}

}

// Initialize Actor data given info from a JSON document
void Actor::initialize(const rapidjson::Document &doc) {

	// Pass into real initialize (above)
	initialize(doc, 0, -1);

}

// Initialize Actor data given another Actor
void Actor::initialize(const Actor &to_copy) {

	// Copy name
	name = to_copy.name;

	// Make instances of components...

	// For every component in the actor to copy...
	for (const std::string &key_to_copy : to_copy.alphabetical_components) {

		// Pull the component to copy's LuaRef
		std::shared_ptr<luabridge::LuaRef> base_ref = ComponentManager::get_component(to_copy.components.at(key_to_copy).type);

		// Make an empty ref
		std::shared_ptr<luabridge::LuaRef> new_ref = std::make_shared<luabridge::LuaRef>(luabridge::newTable(LuaManager::get_lua_state()));

		// Establish the new instance's inheritance
		ComponentManager::establish_inheritance(*new_ref, *base_ref);

		// Copy over all the parameters from the template's components
		for (const auto &pair : luabridge::pairs(*to_copy.components.at(key_to_copy).ref)) {

			(*new_ref)[pair.first] = pair.second;

		}

		// Add the key to the instance (replaces if already read in above)
		new_ref->push(LuaManager::get_lua_state());
		lua_pushstring(LuaManager::get_lua_state(), "key");
		lua_pushstring(LuaManager::get_lua_state(), (*to_copy.components.at(key_to_copy).ref)["key"].cast<std::string>().c_str());
		lua_rawset(LuaManager::get_lua_state(), -3);

		// Add enabled flag to the instance, initialize to false (replaces if already read in above)
		lua_pushstring(LuaManager::get_lua_state(), "enabled");
		lua_pushboolean(LuaManager::get_lua_state(), false);
		lua_rawset(LuaManager::get_lua_state(), -3);
		lua_pop(LuaManager::get_lua_state(), 1);

		// Store the component to our lists
		components.insert({ key_to_copy, { to_copy.components.at(key_to_copy).type, new_ref } });
		insert_alphabetical(key_to_copy);

	}

}

// Inject this actor's reference into all of its components
// To be done once actors are stored in a single spot in memory
void Actor::inject_references() {

	// Do this for every component this actor has...
	for (auto component : components) {

		insert_convenience_reference(component.second.ref);

	}

}

// Call a given lifecycle function
void Actor::call_lifecycle_function(const std::string &function_name) {

	// Go through components alphabetically...
	for (std::string &key : alphabetical_components) {

		// If this component was removed, skip it
		if (components_to_remove.find(key) != components_to_remove.end()) continue;

		// Get the stored reference
		luabridge::LuaRef &component = *components[key].ref;

		// Call the component's given function (if it is defined) only if it is enabled
		if (component[function_name].isFunction() && component["enabled"]) {

			// Throw an exception if anything happens on the Lua side
			try {
				component[function_name](component);
			}
			catch (luabridge::LuaException &exception) {
				EngineTools::ReportError(name, exception);
			}

		}

	}

}

// Flush all components (before scene switch/actor deletion)
void Actor::flush_components() {

	// Add all the components to the deletion list
	for (auto &comp : components) {

		// Disable the component
		(*comp.second.ref)["enabled"] = false;

		components_to_remove.insert(comp.first);

	}

}