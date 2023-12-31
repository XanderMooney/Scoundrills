#include "PlayerStation.h"

PlayerStation::PlayerStation(std::string texture, glm::vec4 destination, glm::vec4 boundingBox, glm::vec2 anchorPointOffset) : 
	_textureID(Jauntlet::ResourceManager::getTexture(texture).id),
	_collider(Jauntlet::BoxCollider2D(glm::vec2(boundingBox.z, boundingBox.w), glm::vec2(boundingBox.x, boundingBox.y))),
	_destination(destination),
	_anchorPoint(glm::vec2(boundingBox.x + (boundingBox.z * 0.5f) + anchorPointOffset.x, boundingBox.y + (boundingBox.w * 0.5f) + anchorPointOffset.y)) 
{
	// Empty
}

void PlayerStation::draw(Jauntlet::SpriteBatch& spriteBatch) {
	spriteBatch.draw(_destination, _textureID);
}

bool PlayerStation::isColliding(glm::vec2 position) {
	// consider creating the Collision2D in the header
	Jauntlet::Collision2D collision;
	return collision.calcCollision(&_collider, position);
}

bool PlayerStation::isOccupied() const {
	return _occupied;
}

void PlayerStation::occupy() {
	_occupied = true;
}

void PlayerStation::unoccupy() {
	_occupied = false;
}

void PlayerStation::onPlayerArrival(Player& player) {
	// Used by derived classes, but some don't so its intended for this empty method to run. -xm
}

glm::vec2 PlayerStation::getAnchorPoint() const {
	return _anchorPoint;
}

glm::vec4 PlayerStation::getBoundingBox() const {
	return _destination;
}

glm::vec4 PlayerStation::getDisplayBox() const {
	return _occupied ? glm::vec4(_anchorPoint - glm::vec2(32), 64, 64) : _destination;
}