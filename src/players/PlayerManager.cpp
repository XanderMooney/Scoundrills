#include <Jauntlet/Collision/Collision2D.h>

#include "PlayerManager.h"

PlayerManager::PlayerManager() {
	// Empty
}
void PlayerManager::init(int initialPlayers, Jauntlet::TileMap* tileWalls) {
	_pathRenderer.init(tileWalls);

	_players.reserve(sizeof(Player) * initialPlayers);

	for (int i = 0; i < initialPlayers; i++) {
		_players.emplace_back(64 * (i + 1) + 320, -64);
	}
}
void PlayerManager::addPlayer(Player& player) {
	_players.push_back(player);
}
void PlayerManager::createPlayer(int x, int y) {
	_players.emplace_back(x, y);
}

bool PlayerManager::processInput(Jauntlet::InputManager* inputManager, Jauntlet::Camera2D* activeCamera, Jauntlet::TileMap* navTileMap) {
	// if we click
	if (inputManager->isKeyPressed(SDL_BUTTON_LEFT)) {
		if (_selectedPlayer == -1) { // we are selecting a player.
			Jauntlet::Collision2D collision;
			glm::vec2 mousePos = activeCamera->convertScreenToWorld(inputManager->getMouseCoords());

			for (int i = 0; i < _players.size(); i++) {
				if (collision.getCollision(&_players[i].collider, mousePos)) {
					_selectedPlayer = i;
					return true;
				}
			}
		}
		else { // we have selected a position for the player to move to.
			_storedMousePos = activeCamera->convertScreenToWorld(inputManager->getMouseCoords());
			_players[_selectedPlayer].navigateTo(navTileMap, navTileMap->RoundWorldPos(_storedMousePos));
			_pathRenderer.clearPath();
			_selectedPlayer = -1;
		}	
	} else if (_selectedPlayer != -1) {
		if (_storedMousePos != activeCamera->convertScreenToWorld(inputManager->getMouseCoords())) {
			_storedMousePos = activeCamera->convertScreenToWorld(inputManager->getMouseCoords());
			_pathRenderer.createPath(_players[_selectedPlayer].getPosition(), navTileMap->RoundWorldPos(_storedMousePos));
		}
	}
	return false;
}

bool PlayerManager::isPlayerSelected() {
	return _selectedPlayer != -1;
}
void PlayerManager::assignStation(PlayerStation* newStation) {
	_players[_selectedPlayer].assignStation(newStation);
}

void PlayerManager::update() {
	for (int i = 0; i < _players.size(); i++) {
		_players[i].update();
	}
}

void PlayerManager::draw(Jauntlet::SpriteBatch& spriteBatch) {
	for (int i = 0; i < _players.size(); i++) {
		_players[i].draw(spriteBatch);
	}
	_pathRenderer.drawPath();
}