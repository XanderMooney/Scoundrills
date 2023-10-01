#include "CameraManager.h"

#include <Jauntlet/Time.h>

CameraManager::CameraManager() {
	// Empty
}

void CameraManager::init(glm::ivec2 windowSize, Jauntlet::InputManager* inputManager) {
	camera.init(windowSize.x, windowSize.y);
	
	_moveDown.init(inputManager);
	_moveLeft.init(inputManager);
	_moveUp.init(inputManager);
	_moveRight.init(inputManager);

	_moveDown.addKey(SDLK_s, SDLK_DOWN);
	_moveLeft.addKey(SDLK_a, SDLK_LEFT);
	_moveUp.addKey(SDLK_w, SDLK_UP);
	_moveRight.addKey(SDLK_d, SDLK_RIGHT);
}

void CameraManager::Update() {
	if (_moveDown.isDown()) {
		camera.translate(glm::vec2(0, -400 * Jauntlet::Time::getDeltaTime()));
	}
	if (_moveUp.isDown()) {
		camera.translate(glm::vec2(0, 400 * Jauntlet::Time::getDeltaTime()));
	}
	if (_moveLeft.isDown()) {
		camera.translate(glm::vec2(-400 * Jauntlet::Time::getDeltaTime(), 0));
	}
	if (_moveRight.isDown()) {
		camera.translate(glm::vec2(400 * Jauntlet::Time::getDeltaTime(), 0));
	}

	camera.update();
}

void CameraManager::ActivateCamera(Jauntlet::GLSLProgram* glslProgram) {
	camera.setActiveCamera(glslProgram); 
}

void CameraManager::updateCameraSize(int width, int height) {
	camera.updateCameraSize(width, height);
}