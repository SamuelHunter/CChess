#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>
#include "piece_lib.h"
#include "constants.h"


class Board {
public:
	/*
		calls reset()
	*/
	Board();

	/*
		replaces current state of m_board with initial_board from constants.h
		all PieceLib public functions are const, so no need to reset m_plib
	*/
	void reset();

	/*
		@param		current		position of piece before moving
		@param		turn		current turn

		Throws exception if current is off-board, empty, or doesn't belong to the side whose turn it is
	*/
	void validateCurrent(const std::string& current, const int& turn) const;

	/*
		@param		future		position of piece after moving
		@param		turn		current turn

		Throws exception if future is off-board
	*/
	void validateFuture(const std::string& future, const int& turn) const;


	typedef std::vector<std::string>(Board::*listFxn)(const std::string& current) const;

	/*
		@param		current		position of piece before moving

		@return		vector of legal positions that piece at current could move to (may include initial moves; without capturing)
	*/
	std::vector<std::string> listMoves(const std::string& current) const;

	/*
		@param		current		position of piece before capturing

		@return		vector of legal positions that piece at current could capture
	*/
	std::vector<std::string> listCaptures(const std::string& current) const;

	/*
		@param		current		position of piece before moving
		@param		future		position of piece after moving

		@return		true if move is completed, false if turn continues
		Throws exception if move is illegal
	*/
	bool attemptMove(const std::string& current, const std::string& future);

	/*
		prints layout of board to standard output
	*/
	void print() const;

private:
	/*
		@param		pos			position of a piece

		@return					char of piece at pos, cannot be modified: check constants.h
	*/
	const char& pieceAt(const std::string& pos) const;

	/*
		@param		pos			position of a piece

		@return					char of piece initially at pos, cannot be modified: check constants.h
	*/
	const char& pieceInitiallyAt(const std::string& pos) const;

	/*
		@param		pos			position of a piece
		@param		replacement	new char of piece
	*/
	void setPiece(const std::string& pos, const char& replacement);

	/*
		@param		pos			position (in algebraic notation) to offset from
		@param		offset		[offset in forward direction (negative is backwards), offset in right direction (negative is left)]
		@param		side		enum of White or Black; must be supplied for calls in listFuture (moves are planned where the piece is not located yet)

		@return		a new position offset from pos, according to side of piece at pos
	*/
	const std::string offset(const std::string& pos, const std::vector<int>& offset, const int& side) const;

	/*
		@param		pos			position in question

		@return		true if pos is contained within the board dimensions, false otherwise
	*/
	bool onBoard(const std::string& pos) const;


	typedef bool (Board::*restrictionFxn)(const std::string&, const std::string&) const;

	/*
		@param		dummy		dummy argument to fit restrictionFxn typedef; see listFuture
		@param		dummy		dummy argument to fit restrictionFxn typedef; see listFuture

		@return		true always
	*/
	bool acceptAll(const std::string& dummy1 = "", const std::string& dummy2 = "") const;

	/*
		@param		dummy		dummy argument to fit restrictionFxn typedef; see listFuture
		@param		dummy		dummy argument to fit restrictionFxn typedef; see listFuture

		@return		false always
	*/
	bool rejectAll(const std::string& dummy1 = "", const std::string& dummy2 = "") const;

	/*
		@param		pos			position in question
		@param		dummy		dummy argument to fit restrictionFxn typedef; see listFuture

		@return		true if pos has a piece that was never moved from its initial position
	*/
	bool isInitial(const std::string& pos, const std::string& dummy = "") const;

	/*
		@param		pos			position in question
		@param		dummy		dummy argument to fit restrictionFxn typedef; see listFuture

		@return		true if pos is the empty char, false otherwise
	*/
	bool isEmpty(const std::string& pos, const std::string& dummy = "") const;

	/*
		@param		otherPos		position of potential enemy
		@param		pos				position where 'enemy' is determined from

		@return		true if otherPos contains a piece belonging to side other than myPos, false otherwise
	*/
	bool isEnemy(const std::string& otherPos, const std::string& pos) const;

	/*
		Generic function that listMoves and listCaptures are built off of: restricted by offsetKey, reqf, passf, and maxReq

		@param		current		position of piece
		@param		offsetKey	string of key in json to retrieve offsets array from
		@param		reqf		function that all positions returned are required to obey
		@param		passf		function that intermediate positions can pass instead of reqf (but won't be added to return)
		@param		maxReq		maximum number of reqf-satisfying positions that can be returned

		@return		vector of legal positions that piece at current could move to
		Throws exception if char of piece at current is not found in json
	*/
	std::vector<std::string> listFuture(const std::string& current, const std::string& offsetKey, restrictionFxn reqf, restrictionFxn passf, const int& maxReq) const;

	/*
		@param		current		position of piece
		@param		future		possible position of piece
		@param		lf			function that returns a legal list to check future against

		@return		true if move is legal, false if not
	*/
	bool isLegal(const std::string& current, const std::string& future, listFxn lf) const;


	// Member variables
	// ----------------
	/*
		array of pieces at start of play
	*/
	const char m_initial_board[BOARD_SIZE][BOARD_SIZE] = {
		// [0][0] = a1
		{ 'R',  'N',  'B',  'Q',  'K',  'B',  'N',  'R' },
		{ 'P',  'P',  'P',  'P',  'P',  'P',  'P',  'P' },
		{ ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ' },
		{ ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ' },
		{ ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ' },
		{ ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ' },
		{ 'p',  'p',  'p',  'p',  'p',  'p',  'p',  'p' },
		{ 'r',  'n',  'b',  'q',  'k',  'b',  'n',  'r' },
	};

	/*
		array of pieces currently in play using abbreviations from pieces.json
	*/
	char m_board[BOARD_SIZE][BOARD_SIZE];

	/*
		library of piece rules
	*/
	PieceLib m_plib;
};

#endif BOARD_H
