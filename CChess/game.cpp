#include <iostream>		//std::cout
#include <fstream>		//std::ofstream
#include <sstream>		//std::stringstream
#include <iomanip>		//std::put_time
#include <ctime>		//std::time_t
#include "game.h"


Game::Game()
	:m_board(), m_turn(WHITE) {
}

void Game::play() {
	bool gameOver = false;
	Round this_round;	//for history; must be outside move-loop or will be reconstructed
	while (!gameOver) {			//game has no checkmate yet
		m_board.print();
		char cmd;
		std::cout << std::endl << "[m]ove\t[s]ave\t[q]uit" << std::endl;
		while (1) {	//retry until valid command
			try {
				std::cout << "Command:\t";
				std::cin >> cmd;
				std::cin.ignore();	//flush whitespace
				switch (cmd) {
				case 'm':
					move(this_round);
					break;
				case 's':
					save();
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

void Game::move(Round& r) {
	if (m_turn == WHITE) {
		std::cout << std::endl << "White's turn (uppercase)";
	} else if (m_turn == BLACK) {
		std::cout << std::endl << "Black's turn (lowercase)";
	}
	std::string current, future;
	while (1) {	//retry current
		try {
			std::cout << std::endl << "Current:\t\t";
			std::getline(std::cin, current);
			m_board.validateCurrent(current, m_turn);
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
			break;
		} catch (const std::invalid_argument& e) {	//current invalid OR no moves or captures
			std::cout << e.what() << std::endl;
		}
	}
	while (1) {		//retry future
		try {
			std::cout << std::endl << "Future:\t\t\t";
			std::getline(std::cin, future);
			m_board.validateFuture(future, m_turn);		//check if future is feasible
			if (m_board.attemptMove(current, future)) {	//check if future is legal; if no exception thrown, turn is over
				std::vector<std::string> move = { current, future };
				if (m_turn == WHITE) {
					r.white_turn.push_back(move);
					m_turn = BLACK;
				} else if (m_turn == BLACK) {
					r.black_turn.push_back(move);
					m_history.push_back(r);	//white went first, so white turn is over, but so is black's at this point
					r = {};			//reset
					m_turn = WHITE;	//new round begins
				}
			}//otherwise turn is not over and m_turn has not changed
			break;
		} catch (const std::invalid_argument& e) {	//future invalid OR illegal move
			std::cout << e.what() << std::endl;
		}
	}
}

void Game::save() const {
	std::string filename;
	std::cout << "Enter filename to be saved (no extension): ";
	std::getline(std::cin, filename);
	filename += ".json";

	std::ofstream ofs(filename);
	if (ofs.is_open()) {
		json j;
		//store time
		std::stringstream ss;
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		j["time"] = ss.str();
		//store moves in turns in rounds
		for (int i = 0; i < m_history.size(); ++i) {
			j["round"][std::to_string(i)]["white_turn"] = m_history[i].white_turn;
			j["round"][std::to_string(i)]["black_turn"] = m_history[i].black_turn;
		}
		ofs << std::setw(2) << j << std::endl;
		ofs.close();
		std::cout << "Game saved as " << filename << std::endl;
	} else {
		std::cout << "Error creating file! Save failed" << std::endl;
	}
}

bool Game::quit() {
	char quit;
	std::cout << "Confirm: y/n\t";
	std::cin >> quit;
	std::cin.ignore();
	return (quit == 'y');
}
