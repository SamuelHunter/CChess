#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "history.h"


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
		history of rounds/turns/moves
	*/
	History m_history;
};

#endif GAME_H
