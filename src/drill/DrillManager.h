/* Purpose: Handles the drill and the happenings within it.
 * Main Contributer(s): Xander Mooney / Jack Kennedy
 */

#pragma once
#include <Jauntlet/Tiles/TileMap.h>
#include <Jauntlet/Inputs/InputManager.h>
#include "DrillAssetRenderer.h"
#include "PlayerResources.h"
#include "../piloting/Navigation.h"
#include "../interactable/Holdable.h"
#include "../interactable/specificStations/Boiler.h"
#include "../interactable/specificStations/WaterTank.h"
#include "../interactable/specificStations/Forge.h"
#include "../interactable/specificStations/PipeWorkbench.h"
#include "../interactable/specificStations/Fridge.h"

class PlayerManager;
class CameraManager;

const float DISASTER_INTERVAL = 10.0f;

class DrillManager {
public:
	DrillManager(CameraManager* cameraManager, PlayerResources& resourceManager, Jauntlet::Camera2D* camera);
	
	// update parts of the drill
	void update();

	// draw elements that appear beneath the players
	void drawLayerOne();
	// draw elements that appear above the players
	void drawLayerTwo();

	// turn on the drill
	void on();
	// turn off the drill
	void off();
	// toggle the drill
	void toggle();
	// returns true if the drill is on
	bool isOn();

	bool isValidDestination(glm::vec2 worldPos, PlayerManager* playerManager) const;
	bool isValidPath(glm::vec2 worldPos, PlayerManager* playerManager) const;
	bool isValidPath(glm::vec2 worldPos) const;
	bool isSteeringWheelOccupied() const;

	PlayerStation* checkHoveringStation(glm::vec2 position);
	bool doesTileOverlapStations(glm::ivec2 tilePos) const;

	void burstRandomPipe();
	void burstSpecificPipe(const glm::ivec2& tilePos);
	void repairPipe(const glm::vec2& worldPos);
	bool DestMatchesRandomPipe(const glm::vec2& worldPos) const;

	// adds a holdable item to be managed by the drillManager.
	Holdable* addHoldable(const glm::vec2& position, const HoldableType& type = HoldableType::NONE);
	// removes the holdable from the drillManagers references.
	void removeHoldable(Holdable* holdable);
	// gives you a pointer to a holdable if it matches the given position
	Holdable* getHoldable(glm::vec2 worldPos);
	// Returns all holdable items
	std::vector<Holdable*> getAllHoldables() const;

	Jauntlet::TileMap drillWalls = Jauntlet::TileMap(_textureCache, 64);
	Jauntlet::TileMap drillFloor = Jauntlet::TileMap(_textureCache, 64);
	Jauntlet::TileMap pipes = Jauntlet::TileMap(_textureCache, 64);

	float boilerWater = Boiler::BOILER_MAX_WATER;
	Navigation navigation;
	PlayerResources* resources;
	Boiler boiler;
	DrillAssetRenderer drillAssets;
	Forge forge;
private:
	// Commits a random disaster
	void DisasterEvent();
	// places an ice item on the ground
	void placeIce();
	// places scrap on the ground
	void placeScrap();

	// Drill assets / Pieces
	WaterTank _waterTank;
	PipeWorkbench _pipeWorkbench;
	Fridge _fridge = Fridge(glm::vec2(8*64, -64), resources);

	// Texture cache for tilemaps
	Jauntlet::TextureCache _textureCache;

	// Drill States
	bool _drillOn = true;
	float _disasterTime = DISASTER_INTERVAL;

	// Holdable Item management
	std::vector<Holdable*> _holdables;
	Jauntlet::SpriteBatch _spriteBatch;

	// Broken pipes
	std::vector<glm::ivec2> _brokenPipeLocations;

	CameraManager* _cameraManager;
};