#ifndef PIECE_LIB_H
#define PIECE_LIB_H

#include <string>
#include <vector>
#include "constants.h"
#include <nlohmann/json.hpp>
// for convenience
using json = nlohmann::json;


class PieceLib {
public:
	/*
		loads json where pieces are stored
	*/
	PieceLib();

	/*
		@param		piece		char of piece (case unimportant)

		@return		true if there is an object "piece" in the json
	*/
	bool contains(const char& piece) const;


	/*
		@param		piece			char of piece (case unimportant)
		@param		capitalization	converted to uppercase for White, lowercase for Black

		@return		name of piece from json, possibly with capitalization by side
	*/
	const std::string getName(const char& piece, bool capitalization = true) const;

	/*
		@param		piece		char of piece (case unimportant)
		@param		offsetKey	type of offsets, e.g. move or capture

		@return		vector of offsets of type offsetKey
	*/
	const std::vector<std::vector<int>> getOffsets(const char& piece, const std::string& offsetKey) const;

private:
	/*
		@param		piece		char of piece

		@return		json on rules pertaining to piece at pos (cannot be modified)
		Used for all public functions to shorten syntax
	*/
	const json& getRules(const char& piece) const;


	// Member variables
	// ----------------
	/*
	json loaded from file containing piece rules
	*/
	json m_library;
};

#endif PIECE_LIB_H
