#include "MainGame.h"

// initialize the window via an initializer list
MainGame::MainGame() 
	: 
	_window("Jauntlet Game Engine", _screenWidth, _screenHeight, 0),
	_camera(_screenWidth, _screenHeight) 
{
	// Empty
}

void MainGame::run() {
	initSystems();
	gameLoop();
}

void MainGame::initSystems() {
	// initialize Jauntlet
	Jauntlet::init();

	// set some default window properties
	_window.setBackgroundColor(Jauntlet::Color(76, 24, 32));
	_window.setWindowIcon("Textures/Icon.png");
	
	// set default missing Texture
	Jauntlet::ResourceManager::setMissingTexture("Textures/Icon.png");
	
	// initialize shaders
	initShaders();
}

void MainGame::initShaders() {
	// compile a vertex and fragment shader, with properties to be able to render simple objects.
	_colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

void MainGame::gameLoop() {
	// set the max fps the game can run at. -1 uncaps framerate.
	Jauntlet::Time::setMaxFPS(60);

	while (_gameState == GameState::PLAY) {
		// start a frame
		Jauntlet::Time::beginFrame();
		// process user inputs
		processInput();
		// update the camera
		_camera.update();
		// draw the game
		drawGame();
		// end a frame. This returns the FPS of the game
		_fps = Jauntlet::Time::endFrame();
	}
}

void MainGame::processInput() {
	_inputManager.processInput();
	// runs if the user quits the game through alt+f4, the X in the corner of the window, or any other means
	if (_inputManager.quitGameCalled()) {
		_gameState = GameState::EXIT;
	}
}

void MainGame::drawGame() {
	// Reset screen
	_window.clearScreen();
	// activate shaders
	_colorProgram.use();
	// activate camera
	_camera.setActiveCamera();

	// --> Draw things you want to render here <--

	// disable shaders
	_colorProgram.unuse();
	// put the newly rendered objects on screen
	_window.swapBuffer();
}