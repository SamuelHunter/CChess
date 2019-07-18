#ifndef GAME_H
#define GAME_H

#include "board.h"


class Game {
public:
	/*
		constructs the board
	*/
	Game();


	/*
		loop of move selection and execution
	*/
	void play();
private:
	Board m_board;
	int m_turn;
};

#endif GAME_H
