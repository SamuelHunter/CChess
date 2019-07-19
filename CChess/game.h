#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <deque>

/*
	every round starts with a White turn and ends with a Black turn,
	regardless of how many moves occur
*/
struct Round {
	std::string white_turn;
	std::string black_turn;
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
		@return		true when move is correctly executed

		enter moves and store in history
	*/
	bool move(Round& r);


	/*
		@return		true when save is successful

		saves game history to file
	*/
	bool save() const;


	/*
		@return		true if quit is confirmed
	*/
	bool quit();
private:
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
