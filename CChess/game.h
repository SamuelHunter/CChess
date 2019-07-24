#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "history.h"


class Game {
public:
	/*
		@brief		calls reset()
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

		@throw		invalid_argument if file contains illegal move(s)
	*/
	void load(const std::string& filename, const bool& silent = false);

	/*
		@brief		resets game conditions to start

		@param		newRules		if supplied, rules will be updated as well
	*/
	void reset(const std::string& newRules = "");

private:
	/*
		@return		whether user confirmed the command when prompted
	*/
	bool confirm(const std::string& message = "Confirm") const;

	/*
		@return		filename (without extension) entered by user
	*/
	const std::string requestString(const std::string& message) const;

	/*
		@brief		prints all available moves from current


		@param		current		position chosen to move/capture from

		@throw		std::invalid_argument
	*/
	void listAvailable(const std::string& current);

	//Member variables
	//----------------
	/*
		@brief		board containing layout of pieces, move checking, and rules
					pointer instead of object so rules can be supplied after game object is constructed
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

	/*
		@brief		name of rules game is being played under (from ruleset.json)
	*/
	std::string m_rules_name;
};

#endif GAME_H
