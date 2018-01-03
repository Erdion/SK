#ifndef __GAME_H_INCLUDED__
#define __GAME_H_INCLUDED__

#include <vector>
#include <string>
#include <map>
#include <list>
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
	private:
		int width;
		int height;
		std::vector<std::vector<Field>> board;

	public:
		Board(int width = 13, int height = 9);
		void fillBoard();
		Field getField(int x, int y);
		void setField(int x, int y, Field field);
		std::string getBoardString();
		void print();
		std::pair<int, int> getSize();
	};

	class Player{
	private:
		int index;
		Field playerField;
		int x;
		int y;
		int bombsLeft;
	public:
		Player(int index, int x, int y, int bombs = 3);
		std::pair<int, int> getCoords();
		void setCoords(int x, int y);
		Field getField();
		int getBombsLeft();
		void removeBomb();
		void addBomb();
	};

	class Bomb{
	private:
		int x;
		int y;
		int timeout;
		int startTime;	
	public:
		Bomb(int x, int y, int timeout = 3000);
		int getTimeLeft();
		bool isOnCoords(int x, int y);
	};

	static bool gameInPlay;
	static Board board;
	static std::map<int, Player*> players;
	static std::list<Bomb*> bombs;

	static bool bombOnCoords(int x, int y);

	static void move(int index, Direction direction);
	static void setBomb(int index);
public:
	static void init();
	static void printBoard();
	static std::string getBoardString();
	static void initPlayer(int index);
	static void interpretMessage(std::string message, int index);
	static void removePlayer(int index);

};
#endif
