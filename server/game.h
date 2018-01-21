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

	class Participant{
	private:
		int index;
	public:
		Participant(int index);
		int getIndex();
		void setIndex(int index);
	};

	class Spectator : public Participant{
	public:
		Spectator(int index);
	};

	class Player : public Participant{
	private:
		int x;
		int y;
		Field playerField;
		int range;
		int bombsLeft;
		bool dead;
	public:
		Player(int index, int x, int y, Field playerField, int range = 3, int bombs = 3);
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
		time_point<system_clock> pauseTime;
		Perishable(int x, int y, int timeout);
	public:
		int getTimeLeft();
		void pause();
		void resume();
		std::pair<int, int> getCoords();
		bool isOnCoords(int x, int y);
	};

	class Bomb : public Perishable{
	private:
		int playerIndex;
		int range;
	public:
		Bomb(int x, int y, int playerIndex, int timeout = 3000);
		int getPlayerIndex();
		int getRange();
	};

	class Flame : public Perishable{
	public:
		Flame(int x, int y, int timeout = 2000);
	};
	
	static bool gameInPlay;
	static bool paused;
	static bool ended;
	static std::string whoWonIdentifier;

	static std::list<int> votesFor;

	static Board board;
	static std::map<int, Player*> players;
	static std::map<int, Spectator*> spectators;
	static std::list<Bomb*> bombs;
	static std::list<Flame*> flames;
	static std::map<int, Player*> playerPlayingAs;

	static void startGame();
	static void endGame();
	static void vote(bool start, bool pause, int index);
	static bool hasGameEnded();
	static void pause();
	static void resume();

	static void initPlayer(int index);
	static void initSpectator(int index);
	static void removePlayer(int index);
	static void removeSpectator(int index);
	static void changeSpectatorToPlayer(int index);

	static Bomb* bombOnCoords(int x, int y);
	static bool isBombOnCoords(int x, int y);
	static void explodeCoord(int x, int y);

	static void move(int index, Direction direction);
	static void setBomb(int index); 
	static void explode(Bomb* bomb);
	static void extinguish(Flame* flame);

	static std::string startedGame();
	static std::string pausedGame();
	static std::string numberOfPlayers();
	static std::string endedGame();
	static std::string whoWon();
	static std::string numberOfVoters();

public:
	static void init();
	static void initParticipant(int index);
	static void changeIndex(int prev, int next);
	static void removeParticipant(int index);
	static void printBoard();
	static std::string getBoardString();
	static void interpretMessage(std::string message, int index);
	static void explodeDueBombs();
	static void extinguishDueFlames();
	static int timeUntilPerish();
	static void handleGameEnd();
};
#endif
