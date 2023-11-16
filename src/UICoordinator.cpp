#include <functional>
#include <Jauntlet/UI/UIButtonToggleableElement.h>

#include "UICoordinator.h"

UICoordinator::UICoordinator(Jauntlet::Camera2D* hudCamera, Jauntlet::TextRenderer* textRenderer, Jauntlet::InputManager* inputManager, DrillManager* drillManager, Jauntlet::GLSLProgram* buttonProgram)
:
	_UIManager(hudCamera),
	_fpsPosition(0),
	_fpsColor(0, 255, 0, 255),
	navigation(hudCamera)
{
	_hudCamera = hudCamera;
	_textRenderer = textRenderer;
	_inputManager = inputManager;
	_colorProgram = buttonProgram;
	
	navigation.genNav(_inputManager, buttonProgram);

	_fpsCounter = new Jauntlet::UITextElement(_textRenderer, &fpsText, &_fpsColor, &_fpsPosition);
	_UIManager.addElement(_fpsCounter, &Jauntlet::TextRenderer::textShader);
	//_fpsCounter->visible = _debugging;

	GLuint _buttonTexture = Jauntlet::ResourceManager::getTexture("Textures/button.png").id;
	glm::vec2* buttonPos = new glm::vec2(10, 10);

	// conversion from `void` to `std::function<void ()>` -jk
	std::function<void()> _buttonMethod = std::bind(&DrillManager::toggle, drillManager);

	Jauntlet::UIButtonToggleableElement* _button = new Jauntlet::UIButtonToggleableElement(_inputManager, _buttonMethod, _buttonTexture, buttonPos, glm::vec2(256, 256), Jauntlet::UIElement::ORIGIN_PIN::BOTTOM_LEFT);
	_UIManager.addElement(_button, buttonProgram);

	
	// optimize batches
	_UIManager.optimize();
	_UIManager.resolvePositions();
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
	_UIManager.setScale(((screenSize.y / 1080.0f)));
	_UIManager.resolvePositions();
}

void UICoordinator::toggleDebugMode() {
	_debugging = !_debugging;

	_fpsCounter->visible = _debugging;
}
void UICoordinator::toggleDebugMode(bool debugging) {
	_debugging = debugging;

	_fpsCounter->visible = debugging;
}