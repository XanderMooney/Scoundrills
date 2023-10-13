/* Purpose: Renders assets tied to the drill
 * Main Contributer(s): Xander Mooney / Jack Kennedy
 */
#pragma once
#include <Jauntlet/Rendering/SpriteBatch.h>
#include <Jauntlet/Rendering/ResourceManager.h>
#include <Jauntlet/Rendering/Animation.h>
#include "PlayerStation.h"

class DrillAssetRenderer {
public:
	DrillAssetRenderer();
	// initialize the drill renderer
	void init();
	// draw the first layer of assets; this renders beneath the walls/floor of the drill
	void drawLayerOne();
	// draw the second layer of assets; this renders above the walls/floor of the drill
	void drawLayerTwo();
	// stop the drill animation
	void startAnimation();
	// start the drill animation
	void stopAnimation();
	
	PlayerStation steeringWheel;
private:
	Jauntlet::SpriteBatch _spriteBatch;
	Jauntlet::Animation _drillAnimation;
	bool _shake = false;

};