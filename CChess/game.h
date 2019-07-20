#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <deque>


/*
	every round starts with a White turn and ends with a Black turn,
	regardless of how many moves (2 element vector of strings: from, to) occur
*/
struct Round {
	
	std::vector<std::vector<std::string>> white_turn;
	std::vector<std::vector<std::string>> black_turn;
};

class Game {
public:
	/*
		constructs the board; White has first turn
	*/
	Game();

	/*
		loop of move, save, quit
	*/
	void play();
	
	/*
		enter moves and store in history
	*/
	void move();

	/*
		prints history to standard output
	*/
	void history() const;

	/*
		saves game history to file
	*/
	void save() const;

	/*
		@return		true if moves in file are valid and loaded, false otherwise

		loads game history file and inputs moves
	*/
	bool load();

	/*
		resets game conditions to start
	*/
	void reset();

	/*
		@return		true if quit is confirmed
	*/
	bool quit();

private:
	/*
		@param		current		position chosen to move/capture from

		prints all available moves from current
		throws exception if none exist and another current must be chosen
	*/
	void listAvailable(const std::string& current) const;

	//Member variables
	//----------------
	/*
		board containing layout of pieces, move checking, and rules
	*/
	Board m_board;

	/*
		enum of whose turn it currently is (White or Black)
	*/
	int m_turn;

	/*
		history of moves, stored in Rounds
	*/
	std::deque<Round> m_history;
};

#endif GAME_H
