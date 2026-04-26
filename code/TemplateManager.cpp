// TemplateManager class definition file (TemplateManager.cpp)
// Reads in and stores template actors

#include "TemplateManager.h"

// TemplateManager initializer
// Reads in all templates from resources/actor_templates
void TemplateManager::initialize() {

	// Path to templates folder
	const char* templates_path = "resources/actor_templates/";

	// If there aren't any templates defined, no need to do anything else
	if (!std::filesystem::exists(templates_path)) return;

	// Next, if the folder is empty, same thing
	if (std::filesystem::is_empty(templates_path)) return;

	// Iterate through every file in the templates directory...
	for (const auto &template_file : std::filesystem::directory_iterator(templates_path)) {

		// Get template document using rapidJSON
		rapidjson::Document current_template_doc;

		// If the read operation fails (for whatever reason) exit
		if (!JSONReader::read_json(template_file.path().string().c_str(), current_template_doc)) {
			std::cout << "Error reading this template file???";
			exit(0);
		}

		// Now we have the document...

		// Actor to store this template's data
		Actor new_template_actor;

		// Initialize the actor with the info from this document
		new_template_actor.initialize(current_template_doc);

		// Add this actor to the template list with the key as the filename
		template_list.insert({ template_file.path().stem().string(), new_template_actor });

	}

	return;

}

// Check if a template exists given its name
// If it does, assign the given actor to its values and return true
// If not, return false
bool TemplateManager::check_and_assign_template(const std::string &template_name, Actor &actor) {

	// If it doesn't exist, return false
	if (template_list.find(template_name) == template_list.end()) return false;

	// Otherwise, assign and return true
	actor.initialize(template_list.at(template_name));
	return true;

}