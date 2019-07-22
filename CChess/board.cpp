#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#include "board.h"


// Public
// ------
Board::Board(const std::string& initial) : m_plib(), m_initial_name(initial) {
	reset();
}

void Board::reset() {
	//load initial board from json
	std::ifstream ifs(RULES_DIR + INITIAL_BOARD + JSON_EXT);
	json ib = json::parse(ifs)[m_initial_name];	//only need json of chosen name
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			m_board[i][j] = ib[i][j].get<std::string>()[0];	//copy initial to board (length 1 string is a char)
			m_neverMoved[i][j] = (m_board[i][j] != EMPTY);	//reset neverMoved; only positions with pieces are eligible
		}
	}
}

void Board::validateCurrent(const std::string& current, const int& turn) const {
	if (!onBoard(current)) {
		throw std::invalid_argument("Current coordinates are invalid. Try again.");
	} else if (isEmpty(current)) {
		throw std::invalid_argument("Current coordinates are empty. Try again.");
	} else if (whichSide(pieceAt(current)) != turn) {
		throw std::invalid_argument("Not one of your pieces. Try again.");
	}
}

void Board::validateFuture(const std::string& future, const int & turn) const {
	if (!onBoard(future)) {
		throw std::invalid_argument("Future coordinates are invalid. Try again.");
	}
}

std::vector<std::string> Board::listMoves(const std::string& current) const {
	//no allowable intermediates
	//maximum positions that can be travelled is 1 less than the board size
	std::vector<std::string> combinedMoves = listFuture(current, PIECE_MOVE_ARRAY, &Board::isEmpty, &Board::rejectAll, BOARD_SIZE - 1);
	if (neverMovedAt(current)) {	//add any additional initial moves
		for (const std::string& initial : listFuture(current, PIECE_INITIAL_ARRAY, &Board::isEmpty, &Board::rejectAll, BOARD_SIZE - 1)) {
			combinedMoves.push_back(initial);
		}
	}
	return combinedMoves;
}

std::vector<std::string> Board::listCaptures(const std::string& current) const {
	//an empty position is an allowable intermediate
	return listFuture(current, PIECE_CAPTURE_ARRAY, &Board::isEnemy, &Board::isEmpty, 1);
}

bool Board::attemptMove(const std::string& current, const std::string& future, const bool& silent) {
	if (isLegal(current, future, &Board::listMoves)) {
		if (!silent) {
			std::cout << "> " << m_plib.getName(pieceAt(current)) << " moved from " << current << " to " << future << "." << std::endl;
		}
		setPiece(future, pieceAt(current));
		setPiece(current, EMPTY);
		setNeverMovedAt(current, false);	//no initial move can be made from current or future now
		setNeverMovedAt(future, false);
		return true;	//single turn over; TODO: count down multiple turns
	} else if (isLegal(current, future, &Board::listCaptures)) {
		if (!silent) {
			std::cout << "> " << m_plib.getName(pieceAt(current)) << " at " << current << " captured "
				<< m_plib.getName(pieceAt(future)) << " at " << future << std::endl;
		}
		setPiece(future, pieceAt(current));
		setPiece(current, EMPTY);
		setNeverMovedAt(current, false);
		setNeverMovedAt(future, false);
		return true;
	} else {
		throw std::invalid_argument("Illegal move. Try again.");
	}
}

void Board::print() const {
	std::cout << "  + --------------- +" << std::endl;
	for (int i = BOARD_SIZE - 1; i >= 0; --i) {	//reverse order so row 1 prints last
		std::cout << char(i + FIRST_ROW) << " | ";	//row label
		for (int j = 0; j < BOARD_SIZE; ++j) {
			//can substitute with m_neverMoved to check if initial moves are allowed when appropriate
			std::cout << m_board[i][j];
			if (j == BOARD_SIZE - 1)
				std::cout << " |" << std::endl;
			else
				std::cout << ' ';
		}
	}
	std::cout << "  + --------------- +" << std::endl << "    ";	//col label
	for (char c = 0; c < BOARD_SIZE; ++c) {
		std::cout << char(c + FIRST_COL);
		if (c == BOARD_SIZE - 1)
			std::cout << std::endl;
		else
			std::cout << ' ';
	}
}

// Private
// -------
const char& Board::pieceAt(const std::string& pos) const {
	return m_board[pos[1] - FIRST_ROW][pos[0] - FIRST_COL];
}

void Board::setPiece(const std::string& pos, const char& replacement) {
	m_board[pos[1] - FIRST_ROW][pos[0] - FIRST_COL] = replacement;
}

const bool& Board::neverMovedAt(const std::string& pos) const {
	return m_neverMoved[pos[1] - FIRST_ROW][pos[0] - FIRST_COL];
}

void Board::setNeverMovedAt(const std::string& pos, const bool& replacement) {
	m_neverMoved[pos[1] - FIRST_ROW][pos[0] - FIRST_COL] = replacement;
}

const std::string Board::offset(const std::string& pos, const std::vector<int>& offset, const int& side) const {
	std::string result;
	result += char(pos[0] + offset[1]);
	if (side == WHITE)
		result += char(pos[1] + offset[0]);
	else if (side == BLACK)	//face opposite direction
		result += char(pos[1] - offset[0]);
	return result;
}

bool Board::onBoard(const std::string& pos) const {
	return (pos.size() == 2 &&
		pos[0] >= FIRST_COL && pos[0] < char(FIRST_COL + BOARD_SIZE) &&
		pos[1] >= FIRST_ROW && pos[1] < char(FIRST_ROW + BOARD_SIZE));
}

bool Board::acceptAll(const std::string & dummy1, const std::string & dummy2) const {
	return true;
}

bool Board::rejectAll(const std::string & dummy1, const std::string & dummy2) const {
	return false;
}

bool Board::isEmpty(const std::string& pos, const std::string& dummy) const {
	return pieceAt(pos) == EMPTY;
}

bool Board::isEnemy(const std::string& otherPos, const std::string& pos) const {
	return ((pieceAt(otherPos) != EMPTY) && (whichSide(pieceAt(otherPos)) != whichSide(pieceAt(pos))));
}

std::vector<std::string> Board::listFuture(const std::string& current, const std::string& offsetKey, restrictionFxn reqf, restrictionFxn passf, const int& maxReq) const {
	std::vector<std::string> future;

	// Identify piece char from json
	std::vector<std::vector<int>> offsets;
	if (!m_plib.contains(pieceAt(current))) {
		throw std::invalid_argument("Unidentified piece on the board. No available moves.");
	} else {
		offsets = m_plib.getOffsets(pieceAt(current), offsetKey);
	}

	// Calculate offsets
	int side = whichSide(pieceAt(current));
	for (auto& o : offsets) {
		std::string next = offset(current, o, side);
		if (o.size() == JSON_RANGE_INDEX + 1 && o[JSON_RANGE_INDEX] == JSON_RANGE_INFINITE) {
			//infinite offset, with up to d nexts satisfying rf
			for (int d = maxReq; (d > 0 && onBoard(next)); next = offset(next, o, side)) {
				//next meets restriction and belongs to future
				if ((this->*reqf)(next, current)) {		//member function pointer syntax
					future.push_back(next);
					d--;
				}//next isn't an allowable intermediate position
				else if (!(this->*passf)(next, current)) {
					break;
				}
			}
		} else {
			//regular offset
			if (onBoard(next) && (this->*reqf)(next, current))
				future.push_back(next);
		}
	}
	return future;
}

bool Board::isLegal(const std::string& current, const std::string& future, listFxn lf) const {
	for (std::string move : (this->*lf)(current)) {
		if (future == move) {
			return true;
		}
	}
	return false;
}
