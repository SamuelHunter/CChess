#include <iostream>		// std::cout
#include <fstream>		// std::ofstream

#include "game.h"


Game::Game()
	:m_board(), m_turn(FIRST_TURN), m_history() {
}

void Game::play() {
	//Startup
	std::cout << "Welcome to CChess" << std::endl;
	std::cout << std::endl << "Press ENTER to begin a new game in " << RULES_PRESET << " mode";
	std::cin.get();
	system("cls");
	//Commands
	bool gameOver = false;
	while (!gameOver) {			//TODO:checkmate
		m_board.print();
		
		std::cout << std::endl << "[m]ove  [h]istory  [s]ave  [l]oad  [u]ndo  [r]eset  [q]uit" << std::endl;
		while (1) {	//retry until valid command
			try {
				char cmd;
				std::cout << "Command:\t";
				std::cin >> cmd;
				std::cin.ignore();	//flush whitespace
				switch (cmd) {
				case 'm':
					move();
					break;
				case 'h':
					m_history.print();
					break;
				case 's':
					m_history.save(requestFilename());
					break;
				case 'l':
					if (!load(requestFilename())) reset();
					break;
				case 'u':
					if (m_history.erase(1)) {//only undo once
						m_history.save(UNDO_TEMP, true);		//all silently
						load(UNDO_TEMP, true);
						m_history.deleteSave(UNDO_TEMP, true);
					} else {
						std::cout << "No moves to undo!" << std::endl;
					}
					break;
				case 'r':
					if (confirm()) reset();
					break;
				case 'q':
					if (confirm()) gameOver = true;
					break;
				default:
					throw std::invalid_argument("Unrecognized command. Try again.");
				}
				std::cout << std::endl << "Press ENTER to continue... ";
				std::cin.get();			//wait
				system("cls");
				break;
			} catch (std::invalid_argument& e) {
				std::cout << e.what() << std::endl;
			}
		}

	}
	//Close
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
			if (m_board.attemptMove(current, future)) {	//check if future is legal
				//turn is over
				if (m_turn == WHITE) {
					m_history.recordMove(m_turn, current, future);	//record before m_turn changes
					m_turn = BLACK;
				} else if (m_turn == BLACK) {
					m_history.recordMove(m_turn, current, future, true);	//both turns have finished - last move of turn
					m_turn = WHITE;	//new round begins
				}
			} else {
				//turn is not over and m_turn has not changed
				m_history.recordMove(m_turn, current, future);
			}
			break;
		} catch (const std::invalid_argument& e) {	//future invalid OR illegal move
			std::cout << e.what() << std::endl;
		}
	}
}

bool Game::load(const std::string& filename, const bool& silent) {
	std::string path = SAVE_DIR + filename + JSON_EXT;
	std::ifstream ifs(path);
	json file = json::parse(ifs);
	reset();
	//streamlined version of move()
	try {
		for (int i = 0; i < file["round"].size(); ++i) {	//rounds
			for (const auto& m : file["round"][std::to_string(i)]["white_turn"]) {	//moves
				m_board.validateCurrent(m[0], m_turn);
				m_board.validateFuture(m[1], m_turn);
				if (m_board.attemptMove(m[0], m[1], true)) {	//always silent
					m_history.recordMove(m_turn, m[0], m[1]);
					m_turn = BLACK;
				} else {
					m_history.recordMove(m_turn, m[0], m[1]);
				}
			}
			for (const auto& m : file["round"][std::to_string(i)]["black_turn"]) {
				m_board.validateCurrent(m[0], m_turn);
				m_board.validateFuture(m[1], m_turn);
				if (m_board.attemptMove(m[0], m[1], true)) {	//always silent
					m_history.recordMove(m_turn, m[0], m[1], true);
					m_turn = WHITE;
				} else {
					m_history.recordMove(m_turn, m[0], m[1]);
				}
			}
		}
		if (!silent) {
			std::cout << "Game successfully loaded from " << path << std::endl;
		}
		return true;
	} catch (const std::invalid_argument& e) {
		std::cout << e.what() << std::endl;
		std::cout << path << " contains invalid move(s). Game will be reset." << std::endl;	//TODO:resotre to previous state using copy constructors
		return false;
	}
}

void Game::reset() {
	//matches constructor
	m_board.reset();
	m_turn = FIRST_TURN;
	m_history.reset();
}

bool Game::confirm() const {
	char reset;
	std::cout << "Confirm: y/n\t";
	std::cin >> reset;
	std::cin.ignore();
	return (reset == 'y');
}

const std::string Game::requestFilename() const {
	std::string filename;
	std::cout << "Enter filename (no extension): ";
	std::getline(std::cin, filename);
	return filename;
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
