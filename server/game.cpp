#include <vector>
#include <cstdio>
#include <utility>
#include <map>
#include <cstring>

#include "game.h"

Game::Board Game::board;
std::map<int, Game::Player*> Game::players;

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

void Game::Board::print(){
	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			switch(board[x][y]){
				case PLAYER1:
					printf("1");
					break;
				case PLAYER2:
					printf("2");
					break;
				case PLAYER3:
					printf("3");
					break;
				case PLAYER4:
					printf("4");
					break;
				case BOMB:
					printf("B");
					break;
				case DESTRUCTIBLE:
					printf("D");
					break;
				case WALL:
					printf("#");
					break;
				case EMPTY:
					printf(".");
					break;
			}
			printf(" ");
		}
		printf("\n");
	}
	fflush(stdout);
}

std::pair<int, int> Game::Board::getSize(){
	return std::pair<int, int>(height, width);	
}

Game::Player::Player(int index, int x, int y): index(index), x(x), y(y){
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


void Game::move(int index, Direction direction){
	printf("%d", index);
	fflush(stdout);
	Player* player = players[index];
	std::pair<int, int> coords = player->getCoords();
	int x = coords.first;
	int y = coords.second;

	std::pair<int, int> size = board.getSize();
	int height = size.first;
	int width = size.second;

	Field field = board.getField(x, y); 
	Field nextField;
	printf("m");
	fflush(stdout);
	
	switch(direction){
		case UP:
			if(x == 0) {
				break;
			}

			nextField = board.getField(x - 1, y);

			if(nextField != EMPTY){
				break;
			}

			board.setField(x, y, EMPTY);
			player->setCoords(x - 1, y);
			board.setField(x - 1, y, player->getField());

			break;

		case DOWN:
			if(x == height - 1) {
				break;
			}

			nextField = board.getField(x + 1, y);

			if(nextField != EMPTY){
				break;
			}

			board.setField(x, y, EMPTY);
			player->setCoords(x + 1, y);
			board.setField(x + 1, y, player->getField());

			break;
		case LEFT:
			if(y == 0) {
				break;
			}

			nextField = board.getField(x, y - 1);

			if(nextField != EMPTY){
				break;
			}

			board.setField(x, y, EMPTY);
			player->setCoords(x, y - 1);
			board.setField(x, y - 1, player->getField());
			
			break;
		case RIGHT:
			if(y == width - 1) {
				break;
			}

			nextField = board.getField(x, y + 1);

			if(nextField != EMPTY){
				break;
			}

			board.setField(x, y, EMPTY);
			player->setCoords(x, y + 1);
			board.setField(x, y + 1, player->getField());
			
			break;
	}
}

void Game::init(){
	
}

void Game::printBoard(){
	board.print();
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
