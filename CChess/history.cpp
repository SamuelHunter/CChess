#include <iostream>		// std::cout
#include <fstream>		// std::ofstream
#include <sstream>		// std::stringstream
#include <iomanip>		// std::put_time

#include "constants.h"
#include "history.h"


History::History() {
	reset();		//in future rounds, the end of black's turn will prepare the next Round
}

void History::reset() {
	m_roundCount = 0;
	m_history.clear();
	m_history.push_back(Round());
}

void History::recordMove(const int& turn, const std::string& current, const std::string& future, const bool& last) {
	if (turn == WHITE) {
		if (m_history.back().white_turn.empty()) {
			++m_roundCount;	//new round has begun and will have a move recorded
		}
		m_history.back().white_turn.push_back({ current, future });
	} else if (turn == BLACK) {
		m_history.back().black_turn.push_back({ current, future });
		if (last) {
			m_history.push_back(Round());
		}
	}
}

void History::print() const {
	std::cout << "------------------------" << std::endl;
	//history always has a first Round, so instead check if filled
	if (!m_roundCount) {
		std::cout << "No history!" << std::endl;
	} else {
		std::cout << "Round\tWhite\tBlack" << std::endl;
		for (int i = 0; i < m_roundCount; ++i) {
			std::cout << i << '\t';
			for (auto& m : m_history[i].white_turn) {
				std::cout << m[0] << '-' << m[1] << ' ';
			}
			std::cout << '\t';
			for (auto& m : m_history[i].black_turn) {
				std::cout << m[0] << '-' << m[1] << ' ';
			}
			std::cout << std::endl;
		}
	}
	std::cout << "------------------------" << std::endl;
}

void History::save() const {
	std::string filename;
	std::cout << "Enter filename to be saved (no extension): ";
	std::getline(std::cin, filename);
	std::string path = SAVE_DIR + filename + JSON_EXT;

	std::ofstream ofs(path);
	if (ofs.is_open()) {
		json j;
		//store moves in turns in rounds
		for (int i = 0; i < m_roundCount; ++i) {
			j["round"][std::to_string(i)]["white_turn"] = m_history[i].white_turn;
			j["round"][std::to_string(i)]["black_turn"] = m_history[i].black_turn;
		}
		//store time
		std::stringstream ss;
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		j["time"] = ss.str();
		//write file
		ofs << std::setw(2) << j << std::endl;
		ofs.close();
		std::cout << "Game saved as " << path << std::endl;
	} else {
		std::cout << "Error creating file! Save failed." << std::endl;
	}
}
