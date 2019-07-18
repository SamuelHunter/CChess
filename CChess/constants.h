#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>


enum Side {
	WHITE = 0,
	BLACK = 1
};
const int FIRST_TURN = WHITE;
/*
	@param		piece		char of piece

	@return		enum of White if piece is uppercase; Black otherwise
*/
int inline whichSide(const char& piece) {
	return (piece == toupper(piece)) ? WHITE : BLACK;
}


const std::string PIECES_JSON = "pieces.json";
const std::string JSON_NAME = "name";
const std::string JSON_MOVE_ARRAY = "move";
const std::string JSON_CAPTURE_ARRAY = "capture";
const int JSON_RANGE_INDEX = 2;		//third component of a move vector is range (positive multiplier of first two components; 1 if not specified)
const int JSON_RANGE_INFINITE = -1;	//e.g. [ 1, 1, -1 ] means up-right with unlimited range


const int BOARD_SIZE = 8;
const char FIRST_ROW = '1';
const char FIRST_COL = 'a';
const char EMPTY = ' ';
const char initial_board[BOARD_SIZE][BOARD_SIZE] = {
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

#endif CONSTANTS_H
