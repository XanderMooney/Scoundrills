#include <functional>
#include <iostream>
#include <Jauntlet/UI/UIButtonToggleableElement.h>

#include "UICoordinator.h"

void bruh() {
	std::cout << std::stoi("a");
}

UICoordinator::UICoordinator(glm::ivec2 screenSize, Jauntlet::Camera2D* hudCamera, Jauntlet::SpriteFont* spriteFont, Jauntlet::InputManager* inputManager, DrillManager* drillManager)
:
	_UIManager(hudCamera),
	_fpsPosition(0),
	_fpsColor(0, 255, 0, 255)
{
	_hudCamera = hudCamera;
	_spriteFont = spriteFont;
	_inputManager = inputManager;

	_UIManager.setScale((hudCamera->getCameraSize().y / 1080.0f) * (hudCamera->getCameraSize().x / 1920.0f));
	_UIManager.resolvePositions();

	navigation.genNav(_UIManager, _inputManager);

	_fpsCounter = new Jauntlet::UITextElement(_spriteFont, &fpsText, &_fpsColor, &_fpsPosition);

	_UIManager.addElement(_fpsCounter);

	GLuint _buttonTexture = Jauntlet::ResourceManager::getTexture("Textures/button.png").id;
	glm::vec2* buttonPos = new glm::vec2(10, 10);

	// conversion from `void` to `std::function<void ()>` -jk
	std::function<void()> _buttonMethod = std::bind(&DrillManager::toggle, drillManager);

	Jauntlet::UIButtonToggleableElement* _button = new Jauntlet::UIButtonToggleableElement(_inputManager, &bruh, _buttonTexture, buttonPos, glm::vec2(512, 512), Jauntlet::UIElement::ORIGIN_PIN::BOTTOM_LEFT);

	_UIManager.addElement(_button);
}

UICoordinator::~UICoordinator() {
	delete _fpsCounter;
	delete buttonPos;
	delete _button;
}

void UICoordinator::draw() {
	_UIManager.draw();
}

void UICoordinator::applyNewScreenSize(glm::ivec2 screenSize) {
	_UIManager.setScale((screenSize.y / 1080.0f) * (screenSize.x / 1920.0f));
	_UIManager.resolvePositions();

	// temporary
	navigation.genNav(_UIManager, _inputManager);
}