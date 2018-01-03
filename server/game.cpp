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

Game::Player::Player(int index, int x, int y, int bombs): index(index), x(x), y(y), bombsLeft(bombs){
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

Game::Bomb::Bomb(int x, int y, int playerIndex, int timeout): x(x), y(y), playerIndex(playerIndex), timeout(timeout){
	range = Game::players[playerIndex]->getRange();
	startTime = system_clock::now();
}

int Game::Bomb::getTimeLeft(){
	auto now = system_clock::now();
	duration<double> diff = now - startTime;
	return timeout - (int)(diff.count() * 1000);
}

int Game::Bomb::getPlayerIndex(){
	return playerIndex;
}

bool Game::Bomb::isOnCoords(int x, int y){
	return (this->x == x && this->y == y);
}

std::pair<int, int> Game::Bomb::getCoords(){
	return std::pair<int, int>(x, y);
}

int Game::Bomb::getRange(){
	return range;
}

bool Game::bombOnCoords(int x, int y){
	for(auto bomb: bombs){
		if(bomb->isOnCoords(x, y)){
			return true;
		}
	}
	return false;
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

	if(nextField == EMPTY && !bombOnCoords(nextX, nextY)){
		if(bombOnCoords(x, y)){
			board.setField(x, y, BOMB);
		}
		else{
			board.setField(x, y, EMPTY);
		}
		player->setCoords(nextX, nextY);
		board.setField(nextX, nextY, player->getField());
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

void Game::explode(int x, int y, int range){
	board.setField(x, y, EMPTY);
	std::pair<int, int> size = board.getSize();
	int height = size.first;
	int width = size.second;
	for(int i = -range; i < range + 1; i++){
		if(x - range > 0 && x + range < height - 1){
			
		}
		if(y - range > 0 && y + range < width - 1){

		}
	}
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
	else {

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
		Bomb* bomb = bombs.front();
		std::pair<int, int> coords = bomb->getCoords();
		int x = coords.first;
		int y = coords.second;
		explode(x, y, bomb->getRange());
		players[bomb->getPlayerIndex()]->addBomb();
		bombs.pop_front();
		delete bomb;
	}
}

int Game::timeUntilExplosion(){
	if(bombs.empty()){
		return -1;
	}
	int res = bombs.front()->getTimeLeft();
	if(res <= 0){
		res = 1;
	}
	return res;
}
