#include <iostream>
#include "game.h"


Game::Game()
	:m_board(), m_turn(FIRST_TURN) {
}

void Game::play() {
	while (1) {	//move entering
		m_board.print();
		if (m_turn == WHITE) {
			std::cout << std::endl << "White's turn" << std::endl;
		} else if (m_turn == BLACK) {
			std::cout << std::endl << "Black's turn" << std::endl;
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
					(m_turn == WHITE) ? (m_turn = BLACK) : (m_turn = WHITE);
				}//otherwise turn is not over and m_turn has not changed
				break;
			} catch (const std::invalid_argument& e) {	//future invalid OR illegal move
				std::cout << e.what() << std::endl;
			}
		}
		std::cout << "\033c";	//clear console for windows/linux
	}
}
