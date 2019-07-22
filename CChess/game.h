#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "history.h"


class Game {
public:
	/*
		@brief		constructs the board; White has first turn
	*/
	Game();

	/*
		@brief		loop of move, save, quit
	*/
	void play();
	
	/*
		@brief		enter moves and store in history
	*/
	void move();

	/*
		@brief		loads game history file and inputs moves


		@param		filename		name of file (without extension) to be loaded into history
		@param		silent			if true, won't print success message

		@return		true if moves in file are valid and loaded, false otherwise
	*/
	bool load(const std::string& filename, const bool& silent = false);

	/*
		@brief		resets game conditions to start
	*/
	void reset();

private:
	/*
		@return		whether user confirmed the command when prompted
	*/
	bool confirm() const;

	/*
		@return		filename (without extension) entered by user
	*/
	const std::string requestFilename() const;

	/*
		@brief		prints all available moves from current


		@param		current		position chosen to move/capture from

		@throw		std::invalid_argument
	*/
	void listAvailable(const std::string& current) const;

	//Member variables
	//----------------
	/*
		@brief		board containing layout of pieces, move checking, and rules
	*/
	Board m_board;

	/*
		@brief		enum of whose turn it currently is (White or Black)
	*/
	int m_turn;

	/*
		@brief		history of rounds/turns/moves
	*/
	History m_history;
};

#endif GAME_H
