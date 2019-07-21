#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <string>
#include <deque>

#include <nlohmann/json.hpp>
// for convenience
using json = nlohmann::json;


class History {
public:
	/*
		@brief		calls reset()
	*/
	History();

	/*
		resets state of m_history and m_roundCount
	*/
	void reset();

	/*
		@brief		on the top Round of m_history, pushes back a vector of [current, future] onto the corresponding side's turn

		@param		turn		enum of side whose turn it is
		@param		current		inital position in move
		@param		future		final position in move
		@param		last		true if Black is making last move of turn - causes new Round to be prepared
		
		WARNING: History only records moves; it never checks their validity
	*/
	void recordMove(const int& turn, const std::string& current, const std::string& future, const bool& last = false);

	/*
		@brief		prints history (if any) to standard output
	*/
	void print() const;

	/*
		@brief		saves game history to file
	*/
	void save() const;
	
private:
	/*
		@brief		every round starts with a White turn and ends with a Black turn,
					regardless of how many moves (2 element vector of strings: [from, to]) occur
	*/
	struct Round {

		std::vector<std::vector<std::string>> white_turn;
		std::vector<std::vector<std::string>> black_turn;
	};

	/*
		@brief		history of moves, stored in Rounds by turn
	*/
	std::deque<Round> m_history;

	/*
		@brief		number of recorded round; will differ from m_history.size() whenever black finishes a turn and white hasn't moved yet
	*/
	int m_roundCount;
};

#endif HISTORY_H