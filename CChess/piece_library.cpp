#include <fstream>      // std::ifstream
#include "piece_library.h"

// Public
// ------
PieceLibrary::PieceLibrary() {
	//load piece library from json
	std::ifstream ifs(RULES_DIR + PIECES_LIBRARY + JSON_EXT);
	m_library = json::parse(ifs);
}

bool PieceLibrary::contains(const char & piece) const {
	return !getRules(piece).empty();
}

const std::string PieceLibrary::getName(const char& piece, bool capitalization) const {
	std::string name = getRules(piece)[PIECE_NAME].get<std::string>();
	if (capitalization) {
		if (whichSide(piece) == WHITE) {
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		} else if (whichSide(piece) == BLACK) {
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		}
	}
	return name;
}

const std::vector<std::vector<int>> PieceLibrary::getOffsets(const char & piece, const std::string& offsetKey) const {
	if (getRules(piece)[offsetKey].is_string()) {	//e.g. capture is the same as "move" rather than an explicit array
		//forgive my syntax, for i have sinned
		return getRules(piece)[getRules(piece)[offsetKey].get<std::string>()].get<std::vector<std::vector<int>>>();
	} else {
		return getRules(piece)[offsetKey].get<std::vector<std::vector<int>>>();
	}
}

// Private
// -------
const json& PieceLibrary::getRules(const char& piece) const {
	return m_library[std::string(1, toupper(piece))];
}