#include <vector>
#include <cstdio>
#include <utility>
#include <map>
#include <list>
#include <chrono>
#include <algorithm>

#include "game.h"

using namespace std::chrono;

bool Game::gameInPlay;
bool Game::paused;
bool Game::ended;
std::list<int> Game::votesFor;
std::string Game::whoWonIdentifier;
Game::Board Game::board;
std::map<int, Game::Player*> Game::players;
std::map<int, Game::Spectator*> Game::spectators;
std::map<int, Game::Player*> Game::playerPlayingAs;
std::list<Game::Bomb*> Game::bombs;
std::list<Game::Flame*> Game::flames;

Game::Board::Board(int width, int height) : width(width), height(height), board(height, std::vector<Field>(width, EMPTY)) {
	fillBoard();
}

void Game::Board::fillBoard(){
	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			if(x % 2 == 1 && y % 2 == 1) {
				board[x][y] = WALL;	
			}
			else if((x % 2 == 1 || y % 2 == 1) && (
				!(x == 1 && y == 0) &&
				!(x == 1 && y == width - 1) &&
				!(x == height - 2 && y == 0) &&
				!(x == height - 2 && y == width - 1) &&
				!(x == 0 && y == 1) &&
				!(x == 0 && y == width - 2) &&
				!(x == height - 1 && y == 1) &&
				!(x == height - 1 && y == width - 2)
									)) {
				board[x][y] = DESTRUCTIBLE;
			}
			else{
				board[x][y] = EMPTY;
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
	std::string boardStr = "/b";
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
		boardStr += "N";
	}
	boardStr+="S";
	boardStr+=startedGame();
	boardStr+=pausedGame();
	boardStr+=numberOfPlayers();
	boardStr+=endedGame();
	boardStr+=whoWon();
	boardStr+=numberOfVoters();
	boardStr+="E";
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

Game::Participant::Participant(int index): index(index) {
	
}

int Game::Participant::getIndex(){
	return index;
}

void Game::Participant::setIndex(int index){
	this->index = index;
}

Game::Spectator::Spectator(int index): Participant(index) {
	
}

Game::Player::Player(int index, int x, int y, Game::Field playerField, int range, int bombs): Participant(index), x(x), y(y), playerField(playerField), range(range), bombsLeft(bombs), dead(false){

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

void Game::Perishable::pause(){
	auto now = system_clock::now();
	duration<double> diff = now - startTime;
	timeout -= (int)(diff.count() * 1000);
}

void Game::Perishable::resume(){
	startTime = system_clock::now();
}

std::pair<int, int> Game::Perishable::getCoords(){
	return std::pair<int, int>(x, y);
}

bool Game::Perishable::isOnCoords(int x, int y){
	return (this->x == x && this->y == y);
}

Game::Bomb::Bomb(int x, int y, int playerIndex, int timeout): Perishable(x, y, timeout), playerIndex(playerIndex){
	range = Game::players[playerIndex]->getRange();
}

int Game::Bomb::getPlayerIndex(){
	return playerIndex;
}

int Game::Bomb::getRange(){
	return range;
}

Game::Flame::Flame(int x, int y, int timeout): Perishable(x, y, timeout){
	
}

void Game::startGame(){
	gameInPlay = true;
}

void Game::endGame(){
	bombs.clear();
	flames.clear();
	std::list<int> playersToReinitialize;
	for(auto item: players){
		Player* player = item.second;
		playersToReinitialize.push_back(player->getIndex());
	}
	for(int player: playersToReinitialize){
		removePlayer(player);
	}
	std::list<Spectator*> spectatorsToBePlayers;
	for(auto item: spectators){
		Spectator* spectator = item.second;
		if(spectator->getIndex() <= 4){
			spectatorsToBePlayers.push_back(spectator);
		}
	}

	board.fillBoard();
	for(Spectator* spectator: spectatorsToBePlayers){
		changeSpectatorToPlayer(spectator->getIndex());
	}
	for(int player: playersToReinitialize){
		initPlayer(player);
	}
	ended = true;
	gameInPlay = false;
	paused = false;
}

void Game::vote(bool startBool, bool pauseBool, int index){
	if(players.size() < 2)
		return;
	if(std::find(votesFor.begin(), votesFor.end(), index) == votesFor.end()){
		votesFor.push_back(index);
	}
	if(votesFor.size() * 2 > players.size()){
		if(startBool){
			startGame();
		}
		else {
			if(!pauseBool){
				pause();
			}
			else{
				resume();
			}
		}
		votesFor.clear();
	}
}

bool Game::hasGameEnded(){
	int numberOfAlivePlayers = 0;
	for(auto item: players){
		Player* player = item.second;
		if(!player->isDead()){
			numberOfAlivePlayers++;
		}
	}
	return numberOfAlivePlayers > 1 ? false : true;
}

void Game::pause(){
	paused = true;	
	for(Bomb* &bomb: bombs){
		bomb->pause();
	}
	for(Flame* flame: flames){
		flame->pause();
	}
}

void Game::resume(){
	paused = false;	
	for(Bomb* &bomb: bombs){
		bomb->resume();
	}
	for(Flame* flame: flames){
		flame->resume();
	}
}

void Game::initPlayer(int index){
	Player* player;
	std::pair<int, int> size = board.getSize();
	int height = size.first;
	int width = size.second;
	int playerNumber;
	for(int i = 1; i < 5; i++){
		if(playerPlayingAs.count(i) == 0){
			playerNumber = i;
			break;
		}
	}
	switch(playerNumber){
		case 1:
			player = new Player(index, 0, 0, PLAYER1);
			board.setField(0, 0, PLAYER1);
			break;
		case 2:	
			player = new Player(index, height - 1, width - 1, PLAYER2);
			board.setField(height - 1, width - 1, PLAYER2);
			break;
		case 3:	
			player = new Player(index, height - 1, 0, PLAYER3);
			board.setField(height - 1, 0, PLAYER3);
			break;
		case 4:	
			player = new Player(index, 0, width - 1, PLAYER4);
			board.setField(0, width - 1, PLAYER4);
			break;
	}
	playerPlayingAs[playerNumber] = player;
	players[index] = player;
}

void Game::initSpectator(int index){
	Spectator* spectator = new Spectator(index);
	spectators[index] = spectator;
}


void Game::removePlayer(int index){
	Player* player = players[index];
	std::pair<int, int> coords = player->getCoords();
	int x = coords.first;
	int y = coords.second;

	for(auto& item: playerPlayingAs){
		if(item.second->getIndex() == player->getIndex()){
			playerPlayingAs.erase(item.first);
			break;
		}
	}
	
	board.setField(x, y, EMPTY);
	players.erase(index);
	delete player;
}

void Game::removeSpectator(int index){
	Spectator* spectator = spectators[index];
	spectators.erase(index);
	delete spectator;
}

void Game::changeSpectatorToPlayer(int index){
	removeSpectator(index);
	initPlayer(index);
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
	Flame* flameToExtinguish;
	switch(board.getField(x, y)){
		case PLAYER1:
			playerPlayingAs[1]->die();
			break;
		case PLAYER2:
			playerPlayingAs[2]->die();
			break;
		case PLAYER3:
			playerPlayingAs[3]->die();
			break;
		case PLAYER4:
			playerPlayingAs[4]->die();
			break;
		case BOMB:
			explode(bombOnCoords(x, y));
			break;
		case DESTRUCTIBLE:
			break;
		case FLAME:
			for(auto flame: flames){
				if(flame->isOnCoords(x, y)){
 					flameToExtinguish = flame;
				}
			}
			extinguish(flameToExtinguish);
			break;
		case WALL:
			return;
		case EMPTY:
			break;		
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
	for(auto playerPair: players){
		auto coordsPlayer = playerPair.second->getCoords();
		int xPlayer = coordsPlayer.first;
		int yPlayer = coordsPlayer.second;
		if(xPlayer == x && yPlayer == y){
			playerPair.second->die();
		}
	}
	
	board.setField(x, y, EMPTY);

	std::pair<int, int> size = board.getSize();
	int height = size.first;
	int width = size.second;

	bool wallLeft = false;
	bool wallRight = false;
	bool wallUp = false;
	bool wallDown = false;


	explodeCoord(x, y);
	for(int i = 1; i < range + 1; i++){
		if(x + i <= height - 1){
			if(board.getField(x + i, y) == WALL){
				wallUp = true;
			}
			if(!wallUp){
				if(board.getField(x + i, y) == DESTRUCTIBLE){
					wallUp = true;
				}
				explodeCoord(x + i, y);
			}
		}
		if(x - i >= 0){
			if(board.getField(x - i, y) == WALL){
				wallDown = true;
			}
			if(!wallDown){
				if(board.getField(x - i, y) == DESTRUCTIBLE){
					wallDown = true;
				}
				explodeCoord(x - i, y);
			}
		}
		if(y + i <= width - 1){
			if(board.getField(x, y + i) == WALL){
				wallRight = true;
			}
			if(!wallRight){
				if(board.getField(x, y + i) == DESTRUCTIBLE){
					wallRight = true;
				}
				explodeCoord(x, y + i);
			}
		}
		if(y - i >= 0){
			if(board.getField(x, y - i) == WALL){
				wallLeft = true;
			}
			if(!wallLeft){
				if(board.getField(x, y - i) == DESTRUCTIBLE){
					wallLeft = true;
				}
				explodeCoord(x, y - i);
			}
		}
	}

	flames.sort([](Flame* left, Flame* right){ 
		return left->getTimeLeft() < right->getTimeLeft(); 
	});
	bombs.remove(bomb);
	delete bomb;
}

void Game::extinguish(Flame* flame){
	std::pair<int, int> coords = flame->getCoords();
	int x = coords.first;
	int y = coords.second;
	board.setField(x, y, EMPTY);
	flames.remove(flame);
	delete flame;
}

std::string Game::startedGame(){
	if(gameInPlay)
		return "T";
	return "F";
}

std::string Game::pausedGame(){
	if(paused)
		return "T";
	return "F";
}

std::string Game::numberOfPlayers(){
	return std::to_string(players.size());
}

std::string Game::endedGame(){
	if(ended){
		ended = false;
		return "T";
	}
	return "F";
}

std::string Game::whoWon(){
	return whoWonIdentifier;
}

std::string Game::numberOfVoters(){
	return std::to_string(votesFor.size());	
}

void Game::init(){
	ended = false;
	paused = false;
	whoWonIdentifier = "0";
	gameInPlay = false;
}

void Game::initParticipant(int index){
	if(index <= 4 && !gameInPlay){
		initPlayer(index);
	}
	else{
		initSpectator(index);
	}
}

void Game::changeIndex(int prev, int next){
	if(prev <= 4 && players.count(prev) == 1){
		players[prev]->setIndex(next);
		players[next] = players[prev];
		players.erase(prev);
	}
	else{
		spectators[prev]->setIndex(next);
		spectators[next] = spectators[prev];
		spectators.erase(prev);
	}
}

void Game::removeParticipant(int index){
	if(players.count(index) != 0){
		removePlayer(index);
	}
	else{
		removeSpectator(index);
	}
}

void Game::printBoard(){
	board.print();
}

std::string Game::getBoardString(){
	return board.getBoardString();
}

void Game::interpretMessage(std::string messageFromServer, int index){
	std::string message;
	while(messageFromServer.length() > 0){
		message = messageFromServer.substr(0, 2);
		if(index > 0 && index <= 4 && players.count(index) != 0){
			if(!gameInPlay){
				if(message == "/s"){
					vote(true, false, index);
				}
			}
			else if(!players[index]->isDead()){
				if(paused){
					if(message == "/w"){
						vote(false, true, index);
					}
				}
				else{
					if(message == "/u"){
						move(index, UP);
					}
					else if(message == "/d"){
						move(index, DOWN);
					}
					else if(message == "/l"){
						move(index, LEFT);
					}
					else if(message == "/r"){
						move(index, RIGHT);
					}
					else if(message == "/b"){
						setBomb(index);
					}
					else if(message == "/p"){
						vote(false, false, index);
					}
				}
			}
		}
		if(messageFromServer.length() < 3){
			messageFromServer = "";
		}
		else{
			messageFromServer = messageFromServer.substr(3);
		}
	}
}

void Game::explodeDueBombs(){
	while(!bombs.empty() && !paused && bombs.front()->getTimeLeft() <= 0){
		explode(bombs.front());
	}
}

void Game::extinguishDueFlames(){
	while(!flames.empty() && !paused && flames.front()->getTimeLeft() <= 0){
		extinguish(flames.front());
	}
}

int Game::timeUntilPerish(){
	int res;
	if(paused){
		return -1;
	}
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

void Game::handleGameEnd(){
	if(gameInPlay && hasGameEnded()){
		endGame();
	}
}
