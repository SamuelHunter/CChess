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
	m_moveCount = 0;
	m_history.clear();
	m_history.push_back(Round());
}

const int & History::rounds() {
	return m_roundCount;
}

const int & History::moves() {
	return m_moveCount;
}

void History::recordMove(const int& turn, const std::string& current, const std::string& future, const bool& last) {
	++m_moveCount;
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
	if (!m_moveCount) {
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

void History::save(const std::string& filename, const std::string& rules, const bool& silent) const {
	std::string path = SAVE_DIR + filename + JSON_EXT;
	std::ofstream ofs(path);
	if (ofs.is_open()) {
		json j;
		//store rules
		j[SAVE_RULES] = rules;
		//store moves in turns in rounds
		for (int i = 0; i < m_roundCount; ++i) {
			j[SAVE_ROUND][std::to_string(i)][SAVE_WHITE_TURN] = m_history[i].white_turn;
			j[SAVE_ROUND][std::to_string(i)][SAVE_BLACK_TURN] = m_history[i].black_turn;
		}
		//store time
		std::stringstream ss;
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		j[SAVE_TIME] = ss.str();
		//write file
		ofs << std::setw(2) << j << std::endl;
		ofs.close();
		if (!silent) {
			std::cout << "Game saved at " << j[SAVE_TIME].get<std::string>() << " as " << path << std::endl;
		}
	} else {
		std::cout << "Error creating file! Save failed." << std::endl;
	}
}

void History::deleteSave(const std::string & filename, const bool& silent) const {
	std::string path = SAVE_DIR + filename + JSON_EXT;
	if (remove((path).c_str()) == 0) {
		if (!silent) {
			std::cout << path << " was deleted successfully." << std::endl;
		}
	} else {
		std::cout << "Failed to delete save at " << path << std::endl;
	}
}

bool History::erase(int n) {
	if (m_moveCount == 0) {
		return false;
	}
	for (int i = m_roundCount - 1; i >= 0;) {
		if (n == 0) {	//nothing left to erase
			break;
		} else if (!m_history[i].black_turn.empty()) {	//first clear black's turn
			m_history[i].black_turn.pop_back();
			--m_moveCount;
			--n;
		} else if (!m_history[i].white_turn.empty()) {	//then white's
			m_history[i].white_turn.pop_back();
			--m_moveCount;
			--n;			
		} else if (m_history[i].white_turn.empty()) {	//white's turn is empty, so the round is empty
			--m_roundCount;
			--i;
		}
	}
	return true;	//at least 1 move was erased
}
