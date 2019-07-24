#include <iostream>		// std::cout
#include <fstream>		// std::ofstream

#include "game.h"


Game::Game() {
	reset();
}

void Game::play() {
	//Startup
	std::cout << "Welcome to CChess" << std::endl;
	std::cout << "Please choose from the rule set" << std::endl;
	m_board.printRules();
	while (1) {	//retry rules name
		try {
			reset(requestString("rules name"));
			break;
		} catch (std::invalid_argument& e) {
			std::cout << e.what() << std::endl;
		}
	}
	std::cout << std::endl << "Press ENTER to begin a new game... ";
	std::cin.get();
	system("cls");

	//Commands
	bool playing = false;	//there are commands other than move available after checkmate
	while (!playing) {
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
					m_history.save(requestString("filename (no extension)"), m_rules_name);
					break;
				case 'l':
					load(requestString("filename (no extension)"));
					break;
				case 'u':
					if (m_history.erase(1)) {//only undo 1 move
						m_history.save(UNDO_TEMP, m_rules_name, true);	//all silently
						load(UNDO_TEMP, true);
						m_history.deleteSave(UNDO_TEMP, true);
					} else {
						std::cout << "No moves to undo!" << std::endl;
					}
					break;
				case 'r':
					if (confirm()) {
						if (confirm("Change rules")) {
							while (1) {	//retry rules name
								try {
									reset(requestString("rules name"));
									break;
								} catch (std::invalid_argument& e) {
									std::cout << e.what() << std::endl;
								}
							}
						} else {
							reset();
						}
					}
					break;
				case 'q':
					if (confirm()) playing = true;
					break;
				default:
					throw std::invalid_argument("Unrecognized command. Try again.");
				}
				std::cout << std::endl << "Press ENTER to continue... ";
				std::cin.get();	//wait
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
	if (m_board.preMove(m_turn)) {	//a move can be made
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
}

void Game::load(const std::string& filename, const bool& silent) {
	std::string path = SAVE_DIR + filename + JSON_EXT;
	std::ifstream ifs(path);
	json file = json::parse(ifs);
	reset(file[SAVE_RULES].get<std::string>());
	//streamlined version of move()
	try {
		for (int i = 0; i < file[SAVE_ROUND].size(); ++i) {	//rounds
			for (const auto& m : file[SAVE_ROUND][std::to_string(i)][SAVE_WHITE_TURN]) {	//moves
				m_board.validateCurrent(m[0], m_turn);
				m_board.validateFuture(m[1], m_turn);
				if (m_board.attemptMove(m[0], m[1], true)) {	//always silent
					m_history.recordMove(m_turn, m[0], m[1]);
					m_turn = BLACK;
				} else {
					m_history.recordMove(m_turn, m[0], m[1]);
				}
			}
			for (const auto& m : file[SAVE_ROUND][std::to_string(i)][SAVE_BLACK_TURN]) {
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
			std::cout << "Game successfully loaded from " << path << " (" << file[SAVE_TIME].get<std::string>() << ")" << std::endl;
		}
	} catch (const std::invalid_argument& e) {
		std::cout << e.what() << std::endl;
		std::cout << path << " contains invalid move(s). Game will be reset." << std::endl;	//TODO:resotre to previous state using copy constructors
		reset();
	}
}

void Game::reset(const std::string& newRules) {
	if (!newRules.empty()) {
		m_rules_name = newRules;
	}
	m_board.reset(m_rules_name);	//still ok if empty
	m_turn = FIRST_TURN;
	m_history.reset();
}

bool Game::confirm(const std::string& message) const {
	char reset;
	std::cout << message << "? y/n\t";
	std::cin >> reset;
	std::cin.ignore();
	return (reset == 'y');
}

const std::string Game::requestString(const std::string& message) const {
	std::string filename;
	std::cout << std::endl << "Enter " << message << ":\t";
	std::getline(std::cin, filename);
	return filename;
}

void Game::listAvailable(const std::string & current) {
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
