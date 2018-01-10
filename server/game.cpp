#include <vector>
#include <cstdio>
#include <utility>
#include <map>
#include <list>
#include <chrono>

#include "game.h"

using namespace std::chrono;

Game::Board Game::board;
std::map<int, Game::Player*> Game::players;
std::list<Game::Bomb*> Game::bombs;
std::list<Game::Flame*> Game::flames;

Game::Board::Board(int width, int height) : board(height, std::vector<Field>(width, EMPTY)), width(width), height(height) {
	fillBoard();
}

void Game::Board::fillBoard(){
	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			if(x % 2 == 1 && y % 2 == 1) {
				board[x][y] = WALL;	
			}
			else if(x % 2 == 1 || y % 2 ==  1) {
				//board[x][y] = 'D';
			}
		}
	}
}

Game::Field Game::Board::getField(int x, int y){
	return board[x][y];
}

void Game::Board::setField(int x, int y, Field field){
	board[x][y] = field;
}

std::string Game::Board::getBoardString(){
	std::string boardStr = "";
	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			switch(board[x][y]){
				case PLAYER1:
					boardStr += "1";
					break;
				case PLAYER2:
					boardStr += "2";
					break;
				case PLAYER3:
					boardStr += "3";
					break;
				case PLAYER4:
					boardStr += "4";
					break;
				case BOMB:
					boardStr += "B";
					break;
				case DESTRUCTIBLE:
					boardStr += "D";
					break;
				case WALL:
					boardStr += "#";
					break;
				case FLAME:
					boardStr += "*";
					break;
				case EMPTY:
					boardStr += ".";
					break;
			}
		}
		boardStr += "\n";
	}
	return boardStr;
}

void Game::Board::print(){
	printf("\n");
	std::string boardString = getBoardString();
	printf("%s", boardString.c_str());
	fflush(stdout);
}

std::pair<int, int> Game::Board::getSize(){
	return std::pair<int, int>(height, width);	
}

Game::Player::Player(int index, int x, int y, int range, int bombs): index(index), x(x), y(y), range(range), bombsLeft(bombs), dead(false){
	switch(index){
			break;
		case 1:
			playerField = PLAYER1;
			break;
		case 2:
			playerField = PLAYER2;
			break;
		case 3:
			playerField = PLAYER3;
			break;
		case 4:
			playerField = PLAYER4;
	}
}

void Game::Player::die(){
	dead = true;
}

std::pair<int, int> Game::Player::getCoords(){
	return std::pair<int, int>(x, y);
}

void Game::Player::setCoords(int x, int y){
	this->x = x;
	this->y = y;
}

Game::Field Game::Player::getField(){
	return playerField;
}

int Game::Player::getBombsLeft(){
	return bombsLeft;
}

void Game::Player::removeBomb(){
	bombsLeft--;
}

void Game::Player::addBomb(){
	bombsLeft++;
}

int Game::Player::getRange(){
	return range;
}

void Game::Player::addRange(){
	range++;
}

bool Game::Player::isDead(){
	return dead;
}

Game::Perishable::Perishable(int x, int y, int timeout): x(x), y(y), timeout(timeout){
	startTime = system_clock::now();
}

int Game::Perishable::getTimeLeft(){
	auto now = system_clock::now();
	duration<double> diff = now - startTime;
	return timeout - (int)(diff.count() * 1000);
}

std::pair<int, int> Game::Perishable::getCoords(){
	return std::pair<int, int>(x, y);
}


Game::Bomb::Bomb(int x, int y, int playerIndex, int timeout): Perishable(x, y, timeout), playerIndex(playerIndex){
	range = Game::players[playerIndex]->getRange();
}

int Game::Bomb::getPlayerIndex(){
	return playerIndex;
}

bool Game::Bomb::isOnCoords(int x, int y){
	return (this->x == x && this->y == y);
}

int Game::Bomb::getRange(){
	return range;
}

Game::Flame::Flame(int x, int y, int timeout): Perishable(x, y, timeout){
	
}

Game::Bomb* Game::bombOnCoords(int x, int y){
	for(auto bomb: bombs){
		if(bomb->isOnCoords(x, y)){
			return bomb;
		}
	}
	return NULL;
}

bool Game::isBombOnCoords(int x, int y){
	for(auto bomb: bombs){
		if(bomb->isOnCoords(x, y)){
			return true;
		}
	}
	return false;
}

void Game::explodeCoord(int x, int y){
	switch(board.getField(x, y)){
		case PLAYER1:
			players[1]->die();
			break;
		case PLAYER2:
			players[2]->die();
			break;
		case PLAYER3:
			players[3]->die();
			break;
		case PLAYER4:
			players[4]->die();
			break;
		case BOMB:
			explode(bombOnCoords(x, y));
			break;
		case DESTRUCTIBLE:
			break;
		case WALL:
			return;

	}
	Flame* flame = new Flame(x, y);
	flames.push_back(flame);
	board.setField(x, y, FLAME);
}
	
void Game::move(int index, Direction direction){
	Player* player = players[index];
	std::pair<int, int> coords = player->getCoords();
	int x = coords.first;
	int y = coords.second;

	std::pair<int, int> size = board.getSize();
	int height = size.first;
	int width = size.second;

	Field field = board.getField(x, y); 
	Field nextField;

	int nextX;
	int nextY;

	switch(direction){
		case UP:
			if(x == 0) {
				return;
			}
			nextX = x - 1;
			nextY = y;

			nextField = board.getField(nextX, nextY);

			break;

		case DOWN:
			if(x == height - 1) {
				return;
			}
			nextX = x + 1;
			nextY = y;

			nextField = board.getField(nextX, nextY);

			break;

		case LEFT:
			if(y == 0) {
				return;
			}
			nextX = x;
			nextY = y - 1;

			nextField = board.getField(nextX, nextY);
			
			break;

		case RIGHT:
			if(y == width - 1) {
				return;
			}

			nextX = x;
			nextY = y + 1;

			nextField = board.getField(nextX, nextY);

			break;
	}

	if((nextField == EMPTY || nextField == FLAME) && !bombOnCoords(nextX, nextY)){
		if(bombOnCoords(x, y)){
			board.setField(x, y, BOMB);
		}
		else{
			board.setField(x, y, EMPTY);
		}
		if(nextField == EMPTY){
			player->setCoords(nextX, nextY);
			board.setField(nextX, nextY, player->getField());
		}
		else if(nextField == FLAME){
			player->die();
		}
	}
}

void Game::setBomb(int index){
	Player* player = players[index];
	std::pair<int, int> coords = player->getCoords();
	int x = coords.first;
	int y = coords.second;
	if(player->getBombsLeft() > 0 && !bombOnCoords(x, y)){
		bombs.push_back(new Bomb(x, y, index));
		bombs.sort([](Bomb* left, Bomb* right){ 
			return left->getTimeLeft() < right->getTimeLeft(); 
		});
		player->removeBomb();
	}
}

void Game::explode(Bomb* bomb){
	std::pair<int, int> coords = bomb->getCoords();
	int x = coords.first;
	int y = coords.second;
	int range = bomb->getRange();

	players[bomb->getPlayerIndex()]->addBomb();
	board.setField(x, y, EMPTY);

	std::pair<int, int> size = board.getSize();
	int height = size.first;
	int width = size.second;

	bool wallLeft = false;
	bool wallRight = false;
	bool wallUp = false;
	bool wallDown = false;


	for(int i = 0; i < range + 1; i++){
		if(x + i < height - 1){
			if(board.getField(x + i, y) == WALL){
				wallUp = true;
			}
			if(!wallUp){
				explodeCoord(x + i, y);
			}
		}
		if(x - i > 0){
			if(board.getField(x - i, y) == WALL){
				wallDown = true;
			}
			if(!wallDown){
				explodeCoord(x - i, y);
			}
		}
		if(y + i < width - 1){
			if(board.getField(x, y + i) == WALL){
				wallRight = true;
			}
			if(!wallRight){
				explodeCoord(x, y + i);
			}
		}
		if(y - i > 0){
			if(board.getField(x, y - i) == WALL){
				wallLeft = true;
			}
			if(!wallLeft){
				explodeCoord(x, y - i);
			}
		}
	}

	flames.sort([](Flame* left, Flame* right){ 
		return left->getTimeLeft() < right->getTimeLeft(); 
	});

	delete bomb;
}

void Game::extinguish(Flame* flame){
	std::pair<int, int> coords = flame->getCoords();
	int x = coords.first;
	int y = coords.second;
	board.setField(x, y, EMPTY);
	delete flame;
}

void Game::init(){
	
}

void Game::printBoard(){
	board.print();
}

std::string Game::getBoardString(){
	return board.getBoardString();
}

void Game::initPlayer(int index){
	Player* player;
	std::pair<int, int> size = board.getSize();
	int height = size.first;
	int width = size.second;
	switch(index){
		case 1:
			player = new Player(index, 0, 0);
			board.setField(0, 0, PLAYER1);
			break;
		case 2:	
			player = new Player(index, height - 1, width - 1);
			board.setField(height - 1, width - 1, PLAYER2);
			break;
		case 3:	
			player = new Player(index, height - 1, 0);
			board.setField(height - 1, 0, PLAYER3);
			break;
		case 4:	
			player = new Player(index, 0, width - 1);
			board.setField(0, width - 1, PLAYER4);
			break;
	}
	players[index] = player;
}


void Game::interpretMessage(std::string message, int index){
	if(index > 0 && index <= 4){
		if(!players[index]->isDead()){
			if(message == "/u\n"){
				move(index, UP);
			}
			else if(message == "/d\n"){
				move(index, DOWN);
			}
			else if(message == "/l\n"){
				move(index, LEFT);
			}
			else if(message == "/r\n"){
				move(index, RIGHT);
			}
			else if(message == "/b\n"){
				setBomb(index);
			}
			else if(message == "/s\n"){

			}
			else {

			}
		}
	}
}

void Game::removePlayer(int index){
	Player* player = players[index];
	std::pair<int, int> coords = player->getCoords();
	int x = coords.first;
	int y = coords.second;
	
	board.setField(x, y, EMPTY);
	players.erase(index);
	delete player;
}

void Game::explodeDueBombs(){
	while(!bombs.empty() && bombs.front()->getTimeLeft() <= 0){
		explode(bombs.front());
		bombs.pop_front();
	}
}

void Game::extinguishDueFlames(){
	while(!flames.empty() && flames.front()->getTimeLeft() <= 0){
		extinguish(flames.front());
		flames.pop_front();
	}
}

int Game::timeUntilPerish(){
	int res;
	if(bombs.empty() && flames.empty()){
		return -1;
	}
	else if(bombs.empty()){
		res = flames.front()->getTimeLeft();
	}
	else if(flames.empty()){
		res = bombs.front()->getTimeLeft();
	}
	else{
		res = std::min(bombs.front()->getTimeLeft(), flames.front()->getTimeLeft());
	}
	if(res <= 0){
		res = 1;
	}
	return res;
}

