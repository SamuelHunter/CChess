#ifndef BOARD_H
#define BOARD_H

#include "piece_library.h"
#include "ruleset.h"
#include "constants.h"


class Board {
public:
	/*
		@brief		calls reset()
	*/
	Board(Ruleset* rs);

	/*
		@brief		replaces current state of m_board with m_initial_name from initial_board.json
					all PieceLib public functions are const, so no need to reset m_plib
					similarly for m_rules
	*/
	void reset();

	/*
		@param		current		position of piece before moving
		@param		turn		current turn

		@throw		std::invalid_argument
	*/
	void validateCurrent(const std::string& current, const int& turn) const;

	/*
		@param		future		position of piece after moving
		@param		turn		current turn

		@throw		std::invalid_argument
	*/
	void validateFuture(const std::string& future, const int& turn) const;


	typedef std::vector<std::string>(Board::*listFxn)(const std::string&);

	/*
		@param		current		position of piece before moving

		@return		vector of legal positions that piece at current could move to (may include initial moves; without capturing)
	*/
	std::vector<std::string> listMoves(const std::string& current);

	/*
		@param		current		position of piece before capturing

		@return		vector of legal positions that piece at current could capture
	*/
	std::vector<std::string> listCaptures(const std::string& current);
	
	/*
		@param		side		which side should be "checked" for check?

		@return		true if side is in check (royal piece can be captured by opponent)
	*/
	bool inCheck(const int& side);

	/*
		@param		current		position of piece before moving
		@param		future		position of piece after moving

		@return		true if royal piece would be in check, had this move been executed
	*/
	bool wouldBeCheck(const std::string& current, const std::string& future);

	/*
		@param		side		which side is potentially in checkmate?

		@return		true if side is in checkmate (every move/capture available to side results in check)
	*/
	bool inCheckMate(const int& side);

	/*
		@param		side		whose turn is it?

		@return		true if any move can be made (board not in checkmate)
	*/
	bool preMove(const int& side);

	/*
		@param		current		position of piece before moving
		@param		future		position of piece after moving
		@param		silent		if true, won't print description of what move occurred

		@return		true if move is completed, false if turn continues

		@throw		std::invalid_argument
	*/
	bool attemptMove(const std::string& current, const std::string& future, const bool& silent = false);

	/*
		@brief		prints layout of board to standard output
	*/
	void print() const;

private:
	/*
		@brief		makes backups of board and neverMoved so that what if checks can be carried out non-destructively
	*/
	void freeze();

	/*
		@brief		restores backups of board and neverMoved
		MAKE SURE UNFREEZE WILL BE CALLED BEFORE AN EARLY RETURN!!!!!!!!!!!!!!
	*/
	void unfreeze();

	/*
		@param		pos			position of a piece

		@return		char of piece at pos, cannot be modified: check constants.h
	*/
	const char& pieceAt(const std::string& pos) const;

	/*
		@param		pos			position of a piece
		@param		replacement	new char of piece
	*/
	void setPiece(const std::string& pos, const char& replacement);

	/*
		@param		pos			position

		@return		bool of pos according to m_neverMoved, cannot be modified
	*/
	const bool& neverMovedAt(const std::string& pos) const;

	/*
		@param		pos			position
		@param		replacement	new bool at pos
	*/
	void setNeverMovedAt(const std::string& pos, const bool& replacement);

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

	/*
		@param		side		side whose royal piece must be found

		@return		position of royal piece belonging to side of side
	*/
	const std::string findRoyal(const int& side) const;

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
		@param		otherPos		position of potential enemy
		@param		pos				position where 'enemy' is determined from

		@return		true if otherPos contains a piece belonging to your side, false otherwise
	*/
	bool isFriendly(const std::string& otherPos, const std::string& pos) const;

	/*
		@brief		generic function that listMoves and listCaptures are built off of: restricted by offsetKey, reqf, passf, and maxReq


		@param		current		position of piece
		@param		offsetKey	string of key in json to retrieve offsets array from
		@param		reqf		function that all positions returned are required to obey
		@param		passf		function that intermediate positions can pass instead of reqf (but won't be added to return)
		@param		maxReq		maximum number of reqf-satisfying positions that can be returned

		@return		vector of legal positions that piece at current could move to
		
		@throw		std::invalid_argument
	*/
	std::vector<std::string> listFuture(const std::string& current, const std::string& offsetKey,
		restrictionFxn reqf, restrictionFxn passf, const int& maxReq);

	/*
		@param		current		position of piece
		@param		future		possible position of piece
		@param		legalf		function that returns a legal list to check future against

		@return		true if move is legal, false if not
	*/
	bool isLegal(const std::string& current, const std::string& future, listFxn legalf);

	/*
		@brief		changes char's in board and neverMoved (DOES NOT CHECK FOR MOVE VALIDITY)

		@param		current		position of piece
		@param		future		possible position of piece

	*/
	void execMove(const std::string& current, const std::string& future);

	// Member variables
	// ----------------
	/*
		@brief		array of pieces currently in play using abbreviations from pieces.json
	*/
	char m_board[BOARD_SIZE][BOARD_SIZE];

	/*
		@brief		experimental copy of board (for check tests)
	*/
	char m_board_backup[BOARD_SIZE][BOARD_SIZE];

	/*
		@brief		array of positions where a move has never been played from/to
	*/
	bool m_neverMoved[BOARD_SIZE][BOARD_SIZE];

	/*
		@brief		experimental copy of neverMoved (for check tests)
	*/
	bool m_neverMoved_backup[BOARD_SIZE][BOARD_SIZE];

	/*
		@brief		library of piece rules
	*/
	PieceLibrary m_plib;

	/*
		@brief		rules from ruleset.json
	*/
	Ruleset* m_rules;
};

#endif BOARD_H
