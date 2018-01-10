#ifndef __GAME_H_INCLUDED__
#define __GAME_H_INCLUDED__

#include <vector>
#include <string>
#include <map>
#include <list>
#include <utility>
#include <queue>
#include <chrono>

using namespace std::chrono;

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
		FLAME,
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
		int range;
		bool dead;
	public:
		Player(int index, int x, int y, int range = 3, int bombs = 3);
		void die();
		std::pair<int, int> getCoords();
		void setCoords(int x, int y);
		Field getField();
		int getBombsLeft();
		void removeBomb();
		void addBomb();
		int getRange();
		void addRange();
		bool isDead();
	};

	class Perishable{
	protected:
		int x;
		int y;
		int timeout;
		time_point<system_clock> startTime;	
		Perishable(int x, int y, int timeout);
	public:
		int getTimeLeft();
		std::pair<int, int> getCoords();
	};

	class Bomb : public Perishable{
	private:
		int playerIndex;
		int range;
	public:
		Bomb(int x, int y, int playerIndex, int timeout = 3000);
		int getPlayerIndex();
		bool isOnCoords(int x, int y);
		int getRange();
	};

	class Flame : public Perishable{
	public:
		Flame(int x, int y, int timeout = 2000);
	};
	
	static bool gameInPlay;
	static Board board;
	static std::map<int, Player*> players;
	static std::list<Bomb*> bombs;
	static std::list<Flame*> flames;

	static Bomb* bombOnCoords(int x, int y);
	static bool isBombOnCoords(int x, int y);
	static void explodeCoord(int x, int y);

	static void move(int index, Direction direction);
	static void setBomb(int index); 
	static void explode(Bomb* bomb);
	static void extinguish(Flame* flame);
public:
	static void init();
	static void printBoard();
	static std::string getBoardString();
	static void initPlayer(int index);
	static void interpretMessage(std::string message, int index);
	static void removePlayer(int index);
	static void explodeDueBombs();
	static void extinguishDueFlames();
	static int timeUntilPerish();

};
#endif
