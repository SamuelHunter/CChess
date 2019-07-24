#include <fstream>      // std::ifstream

#include "constants.h"
#include "ruleset.h"

Ruleset::Ruleset(const std::string& rules_name) : m_rules_name(rules_name) {
	//load ruleset from json
	std::ifstream ifs(RULES_DIR + RULESET + JSON_EXT);
	m_ruleset = json::parse(ifs);
}

void Ruleset::setRules(const std::string& name) {
	if (m_ruleset.find(name) != m_ruleset.end()) {
		m_rules_name = name;
	} else {
		throw std::invalid_argument("Rules do no exist. Try again.");
	}
	
}

const char Ruleset::getInitialBoardAt(const int& row, const int& col) const {
	return m_ruleset[m_rules_name][RULES_BOARD][row][col].get<std::string>()[0];
}

const char Ruleset::getRoyal(const int& side) const {
	if (side == WHITE) return toupper(m_ruleset[m_rules_name][RULES_ROYAL].get<std::string>()[0]);
	else return tolower(m_ruleset[m_rules_name][RULES_ROYAL].get<std::string>()[0]);
}

void Ruleset::printAll() const {
	for (const auto& r : m_ruleset.get<json::object_t>()) {
		std::cout << "> " << r.first << std::endl;	//retrieve first field of object (rule names)
	}
}
