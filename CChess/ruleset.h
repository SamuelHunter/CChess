#ifndef RULESET_H
#define RULESET_H

#include <iostream>		// std::cout
#include "constants.h"
#include <nlohmann/json.hpp>
// for convenience
using json = nlohmann::json;


class Ruleset {
public:
	/*
		@brief		loads json where pieces are stored
	*/
	Ruleset(const std::string& rules_name = DEFAULT_RULES);

	/*
		@param		name		name of rules object in json

		@throw		invalid_argument if name is not an object in the json
	*/
	void setRules(const std::string& name);

	/*
		@param		row			row in board array
		@param		col			column in board array

		@return		char of a piece from board array
	*/
	const char getInitialBoardAt(const int& row, const int& col) const;

	/*
		@param		side		for capitalization of royal

		@return		char of royal for side
	*/
	const char getRoyal(const int& side) const;

	void printAll() const;

private:
	/*
		@brief		name of rules object in json
	*/
	std::string m_rules_name;

	/*
		@brief		all rules from json
	*/
	json m_ruleset;
};

#endif RULESET_H