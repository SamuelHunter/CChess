#include <fstream>      // std::ifstream
#include "piece_lib.h"

// Public
// ------
PieceLib::PieceLib() {
	//load pieces from json
	std::ifstream ifs(PIECES_JSON);
	m_library = json::parse(ifs);
}

bool PieceLib::contains(const char & piece) const {
	return !getRules(piece).empty();
}

const std::string PieceLib::getName(const char& piece, bool capitalization) const {
	std::string name = getRules(piece)[JSON_NAME].get<std::string>();
	if (capitalization) {
		if (whichSide(piece) == WHITE) {
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		} else if (whichSide(piece) == BLACK) {
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		}
	}
	return name;
}

const std::vector<std::vector<int>> PieceLib::getOffsets(const char & piece, const std::string& offsetKey) const {
	return getRules(piece)[offsetKey].get<std::vector<std::vector<int>>>();
}

// Private
// -------
const json& PieceLib::getRules(const char& piece) const {
	return m_library[std::string(1, toupper(piece))];
}