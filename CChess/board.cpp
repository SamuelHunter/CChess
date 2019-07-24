#include <iostream>     // std::cout
#include <algorithm>	// std::copy
#include "board.h"


// Public
// ------
Board::Board() : m_plib(), m_rules() {
	reset();
}

void Board::reset(const std::string& newRules) {
	if (!newRules.empty()) {
		m_rules.setRules(newRules);
	}
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			m_board[i][j] = m_rules.getInitialBoardAt(i, j);//copy initial to board (length 1 string is a char)
			m_neverMoved[i][j] = (m_board[i][j] != EMPTY);	//reset neverMoved; only positions with pieces are eligible
		}
	}		//m_board_backup & m_neverMoved_backup can have anything as they are overwritten often
}

void Board::printRules() {
	m_rules.printAll();
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

std::vector<std::string> Board::listMoves(const std::string& current) {
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

std::vector<std::string> Board::listCaptures(const std::string& current) {
	//an empty position is an allowable intermediate
	return listFuture(current, PIECE_CAPTURE_ARRAY, &Board::isEnemy, &Board::isEmpty, 1);
}

bool Board::inCheck(const int& side) {
	for (char row = FIRST_ROW; row < char(FIRST_ROW + BOARD_SIZE); ++row) {
		for (char col = FIRST_COL; col < char(FIRST_COL + BOARD_SIZE); ++col) {
			std::string otherPos;
			otherPos += col;
			otherPos += row;
			if (isEnemy(otherPos, findRoyal(side))) {	//every piece that is an enemy to pos
				for (const auto& c : listCaptures(otherPos)) {	//see if any could capture pos
					if (findRoyal(side) == c) {
						return true;
					}
				}
			}
		}
	}
	return false;	//not at risk
}

bool Board::wouldBeCheck(const std::string& current, const std::string& future) {
	freeze();
	execMove(current, future);
	bool check = inCheck(whichSide(pieceAt(current)));
	unfreeze();	//order matters
	return check;
}

bool Board::inCheckMate(const int& side) {
	for (char row = FIRST_ROW; row < char(FIRST_ROW + BOARD_SIZE); ++row) {
		for (char col = FIRST_COL; col < char(FIRST_COL + BOARD_SIZE); ++col) {
			std::string otherPos;
			otherPos += col;
			otherPos += row;
			if (isFriendly(otherPos, findRoyal(side))) {	//every piece belonging to pos's side
				for (const auto& m : listMoves(otherPos)) {	//check every move to end check
					if (!wouldBeCheck(otherPos, m)) {
						return false;
					}
				}
				for (const auto& c : listCaptures(otherPos)) {	//check every capture to end check
					if (!wouldBeCheck(otherPos, c)) {
						return false;
					}
				}
			}
		}
	}
	return true;	//ded
}

bool Board::preMove(const int& side) {
	//Checkmate?
	if (inCheckMate(side)) {
		std::cout << std::endl << "Checkmate!" << std::endl
			<< m_plib.getName(pieceAt(findRoyal(side))) << ", the royal piece, cannot escape capture." << std::endl;
		if (whichSide(pieceAt(findRoyal(side))) == WHITE) {
			std::cout << "Black wins.";
		} else if (whichSide(pieceAt(findRoyal(side))) == BLACK) {
			std::cout << "White wins.";
		}
		return false;	//game over
	}
	//Check?
	if (inCheck(side)) {
		std::cout << "Warning: " << m_plib.getName(pieceAt(findRoyal(side))) << " is in check." << std::endl;
	}
	if (side == WHITE) {
		std::cout << std::endl << "White's turn (UPPERCASE PIECES)";
	} else if (side == BLACK) {
		std::cout << std::endl << "Black's turn (lowercase pieces)";
	}
	return true;
}

bool Board::attemptMove(const std::string& current, const std::string& future, const bool& silent) {
	if (isLegal(current, future, &Board::listMoves)) {
		if (!silent) {
			std::cout << "> " << m_plib.getName(pieceAt(current)) << " moved from " << current << " to " << future << "." << std::endl;
		}
		execMove(current, future);
		return true;	//single turn over; TODO: count down multiple turns
	} else if (isLegal(current, future, &Board::listCaptures)) {
		if (!silent) {
			std::cout << "> " << m_plib.getName(pieceAt(current)) << " at " << current << " captured "
				<< m_plib.getName(pieceAt(future)) << " at " << future << std::endl;
		}
		execMove(current, future);
		return true;
	} else {	//must fail both move and capture before throwing
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

void Board::freeze() {
	std::copy(&m_board[0][0], &m_board[0][0] + BOARD_SIZE * BOARD_SIZE, &m_board_backup[0][0]);
	std::copy(&m_neverMoved[0][0], &m_neverMoved[0][0] + BOARD_SIZE * BOARD_SIZE, &m_neverMoved_backup[0][0]);
}

void Board::unfreeze() {
	std::copy(&m_board_backup[0][0], &m_board_backup[0][0] + BOARD_SIZE * BOARD_SIZE, &m_board[0][0]);
	std::copy(&m_neverMoved_backup[0][0], &m_neverMoved_backup[0][0] + BOARD_SIZE * BOARD_SIZE, &m_neverMoved[0][0]);
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

const std::string Board::findRoyal(const int& side) const {
	for (char row = FIRST_ROW; row < char(FIRST_ROW + BOARD_SIZE); ++row) {
		for (char col = FIRST_COL; col < char(FIRST_COL + BOARD_SIZE); ++col) {
			std::string pos;
			pos += col;
			pos += row;
			if (pieceAt(pos) == m_rules.getRoyal(side)) {
				return pos;
			}
		}
	}
	return std::string();
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

bool Board::isFriendly(const std::string & otherPos, const std::string & pos) const {
	return ((pieceAt(otherPos) != EMPTY) && (whichSide(pieceAt(otherPos)) == whichSide(pieceAt(pos))));
}

std::vector<std::string> Board::listFuture(const std::string& current, const std::string& offsetKey,
	restrictionFxn reqf, restrictionFxn passf, const int& maxReq) {
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
		for (std::string next = offset(current, o, side); onBoard(next); next = offset(next, o, side)) {
			if (future.size() >= maxReq) {
				break;
			}
			//next meets restriction and belongs to future (member function pointer syntax)
			else if ((this->*reqf)(next, current)) {
				future.push_back(next);
			}
			//next isn't an allowable intermediate position
			else if (!(this->*passf)(next, current)) {
				break;
			}
			//single offset - no more chances
			if (o.size() == JSON_RANGE_INDEX) {	
				break;
			}
		}
	}
	return future;
}

bool Board::isLegal(const std::string& current, const std::string& future, listFxn lf) {
	for (std::string move : (this->*lf)(current)) {
		if (future == move) {
			if (wouldBeCheck(current, future)) {
				throw std::invalid_argument("Move would put " + m_plib.getName(pieceAt(findRoyal(whichSide(pieceAt(current)))))
					+ " in check. Try again.");
			} else {
				return true;
			}
		}
	}
	return false;
}

void Board::execMove(const std::string & current, const std::string & future) {
	setPiece(future, pieceAt(current));
	setPiece(current, EMPTY);
	setNeverMovedAt(current, false);	//no initial move can be made from current or future now
	setNeverMovedAt(future, false);
}
