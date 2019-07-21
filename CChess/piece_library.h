#ifndef PIECE_LIBRARY_H
#define PIECE_LIBRARY_H

#include <nlohmann/json.hpp>
// for convenience
using json = nlohmann::json;


class PieceLibrary {
public:
	/*
		@brief		loads json where pieces are stored
	*/
	PieceLibrary();

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
		@brief		Used for all public functions to shorten syntax


		@param		piece		char of piece

		@return		json on rules pertaining to piece at pos (cannot be modified)
	*/
	const json& getRules(const char& piece) const;


	// Member variables
	// ----------------
	/*
		@brief		json loaded from file containing piece rules
	*/
	json m_library;
};

#endif PIECE_LIBRARY_H
