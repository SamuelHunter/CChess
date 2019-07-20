#include <iostream>		//std::cout
#include <fstream>		//std::ofstream
#include <sstream>		//std::stringstream
#include <iomanip>		//std::put_time
#include <ctime>		//std::time_t
#include "game.h"


Game::Game()
	:m_board(), m_turn(FIRST_TURN), m_history() {
	Round firstRound;		//in future rounds, the end of black's turn will push_back a new Round
	m_history.push_back(firstRound);
}

void Game::play() {
	bool gameOver = false;
	while (!gameOver) {			//game has no checkmate yet
		m_board.print();
		char cmd;
		std::cout << std::endl << "[m]ove  [h]istory  [s]ave  [l]oad  [r]eset  [q]uit" << std::endl;
		while (1) {	//retry until valid command
			try {
				std::cout << "Command:\t";
				std::cin >> cmd;
				std::cin.ignore();	//flush whitespace
				switch (cmd) {
				case 'm':
					move();
					break;
				case 'h':
					history();
					break;
				case 's':
					save();
					break;
				case 'l':
					load();
					break;
				case 'r':
					reset();
					break;
				case 'q':
					if (quit()) gameOver = true;
					break;
				default:
					throw std::invalid_argument("Unrecognized command. Try again.");
				}
				std::cout << std::endl << "Press ENTER to continue... ";
				std::cin.get();			//wait
				std::cout << "\033c";	//clear console for windows/linux
				break;
			} catch (std::invalid_argument& e) {
				std::cout << e.what() << std::endl;
			}
		}

	}
	std::cout << "Thanks for playing!" << std::endl;
}

void Game::move() {
	if (m_turn == WHITE) {
		std::cout << std::endl << "White's turn (UPPERCASE PIECES)";
	} else if (m_turn == BLACK) {
		std::cout << std::endl << "Black's turn (lowercase pieces)";
	}
	std::string current, future;
	while (1) {	//retry current
		try {
			std::cout << std::endl << "Current:\t\t";
			std::getline(std::cin, current);
			m_board.validateCurrent(current, m_turn);
			listAvailable(current);
			break;
		} catch (const std::invalid_argument& e) {	//current invalid OR no moves or captures
			std::cout << e.what() << std::endl;
		}
	}
	while (1) {	//retry future
		try {
			std::cout << std::endl << "Future:\t\t\t";
			std::getline(std::cin, future);
			m_board.validateFuture(future, m_turn);		//check if future is feasible
			if (m_board.attemptMove(current, future)) {	//check if future is legal; if no exception thrown, turn is over
				if (m_turn == WHITE) {
					m_history.back().white_turn.push_back({ current, future });
					m_turn = BLACK;
				} else if (m_turn == BLACK) {
					m_history.back().black_turn.push_back({ current, future });
					m_history.push_back(Round());	//both turns have finished, so set up a record for next round
					m_turn = WHITE;	//new round begins
				}
			}//otherwise turn is not over and m_turn has not changed
			break;
		} catch (const std::invalid_argument& e) {	//future invalid OR illegal move
			std::cout << e.what() << std::endl;
		}
	}
}

void Game::history() const {
	std::cout << "------------------------" << std::endl;
	//history always has a firstRound, so instead check if filled
	if (m_history[0].white_turn.empty() && m_history[0].black_turn.empty()) {
		std::cout << "No history!" << std::endl;
	} else {
		std::cout << "Round\tWhite\tBlack" << std::endl;
		for (int i = 0; i < m_history.size(); ++i) {
			//ignore unfilled round from right before a new white turn
			if (!m_history[i].white_turn.empty() || !m_history[i].black_turn.empty()) {
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
	}
	std::cout << "------------------------" << std::endl;
}

void Game::save() const {
	std::string filename;
	std::cout << "Enter filename to be saved (no extension): ";
	std::getline(std::cin, filename);
	filename += ".json";

	std::ofstream ofs(filename);
	if (ofs.is_open()) {
		json j;
		//store moves in turns in rounds
		for (int i = 0; i < m_history.size(); ++i) {
			//ignore unfilled round from right before a new white turn
			if (!m_history[i].white_turn.empty() || !m_history[i].black_turn.empty()) {
				j["round"][std::to_string(i)]["white_turn"] = m_history[i].white_turn;
				j["round"][std::to_string(i)]["black_turn"] = m_history[i].black_turn;
			}
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
		std::cout << "Game saved as " << filename << std::endl;
	} else {
		std::cout << "Error creating file! Save failed." << std::endl;
	}
}

bool Game::load() {
	std::string filename;
	std::cout << "Enter filename to load (no extension): ";
	std::getline(std::cin, filename);
	filename += ".json";
	std::ifstream ifs(filename);
	json file = json::parse(ifs);
	reset();
	//streamlined version of move()
	try {
		for (int i = 0; i < file["round"].size(); ++i) {	//rounds
			for (const auto& m : file["round"][std::to_string(i)]["white_turn"]) {	//moves
				m_board.validateCurrent(m[0], m_turn);
				m_board.validateFuture(m[1], m_turn);
				if (m_board.attemptMove(m[0], m[1])) {
					//no need to check turn
					m_history.back().white_turn.push_back({ m[0], m[1] });
					m_turn = BLACK;
				}
			}
			for (const auto& m : file["round"][std::to_string(i)]["black_turn"]) {
				m_board.validateCurrent(m[0], m_turn);
				m_board.validateFuture(m[1], m_turn);
				if (m_board.attemptMove(m[0], m[1])) {
					//no need to check turn
					m_history.back().black_turn.push_back({ m[0], m[1] });
					m_history.push_back(Round());	//both turns have finished, so set up a record for next round
					m_turn = WHITE;	//new round begins
				}
			}
		}
		return true;	//success!
	} catch (const std::invalid_argument& e) {
		std::cout << e.what() << std::endl;
		std::cout << "History contains invalid move(s). Game will be returned to previous state." << std::endl;
		return false;
	}
}

void Game::reset() {
	char reset;
	std::cout << "Confirm: y/n\t";
	std::cin >> reset;
	std::cin.ignore();
	//match constructor
	m_board.resetBoard();
	m_turn = FIRST_TURN;
	m_history.clear();
	Round firstRound;
	m_history.push_back(firstRound);
}

bool Game::quit() {
	char quit;
	std::cout << "Confirm: y/n\t";
	std::cin >> quit;
	std::cin.ignore();
	return (quit == 'y');
}

void Game::listAvailable(const std::string & current) const {
	std::vector<std::string> moves = m_board.listMoves(current);
	std::vector<std::string> captures = m_board.listCaptures(current);
	if (moves.size() == 0 && captures.size() == 0) {
		throw std::invalid_argument("No available moves or captures. Try again.");
	}
	std::cout << "Available moves:\t";
	for (std::string m : moves) {
		std::cout << m << ' ';
	}
	std::cout << std::endl << "Available captures:\t";
	for (std::string c : captures) {
		std::cout << c << ' ';
	}
	std::cout << std::endl;
}
