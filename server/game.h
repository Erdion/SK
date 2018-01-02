#ifndef __GAME_H_INCLUDED__
#define __GAME_H_INCLUDED__

#include <vector>
#include <string>
#include <map>
#include <utility>

class Game{ //shouldn't have been static to allow many games on one server, but it isn't needed so I made this static :P
private:
	enum Direction{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	enum Field{
		PLAYER1,
		PLAYER2,
		PLAYER3,
		PLAYER4,
		BOMB,
		DESTRUCTIBLE,
		WALL,
		EMPTY,
	};

	class Board{
	public:

	private:
		int width;
		int height;
		std::vector<std::vector<Field>> board;

	public:
		Board(int width = 13, int height = 9);
		void fillBoard();
		Field getField(int x, int y);
		void setField(int x, int y, Field field);
		void print();
		std::pair<int, int> getSize();
	};

	class Player{
	private:
		int index;
		Field playerField;
		int x;
		int y;
	public:
		Player(int index, int x, int y);
		std::pair<int, int> getCoords();
		void setCoords(int x, int y);
		Field getField();
	};

	static bool gameInPlay;
	static Board board;
	static std::map<int, Player*> players;

	static void move(int index, Direction direction);
public:
	static void init();
	static void printBoard();
	static void initPlayer(int index);
	static void interpretMessage(std::string message, int index);
	static void removePlayer(int index);

};
#endif
