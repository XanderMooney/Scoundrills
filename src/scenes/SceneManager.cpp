#include "SceneManager.h"
#include <Jauntlet/Time.h>
#include "GlobalContext.h"
#include "MainGame.h"

SceneManager::SceneManager() {
    Jauntlet::init();
    Jauntlet::ResourceManager::setMissingTexture("Textures/missing.png");
    
    GlobalContext::initContext();
    
    // Start at specified scene
#if NDEBUG
    // do NOT change, release builds always build to main menu.
    switchScene(GameState::MAINMENU);
#else
    // Feel free to change this to whatever you need for debugging. This will not compile on release builds.
    switchScene(GameState::MAINGAME);
#endif
    gameLoop();
}

void SceneManager::gameLoop() {
    Jauntlet::Time::setMaxFPS(60);

    while (true) {
        Jauntlet::Time::beginFrame();
        GlobalContext::window.clearScreen();

        GlobalContext::inputManager.processInput();

        if (GlobalContext::inputManager.quitGameCalled()) {
            break;
        }

        if (GlobalContext::inputManager.windowResized()) {
            GlobalContext::window.resolveWindowSize();
            GlobalContext::screenSize = GlobalContext::window.getWindowSize();
            
            if (_gameState == GameState::MAINGAME) {
                // run maingame window resize event.
                _mainGame->windowResized();
            }
            else if (_gameState == GameState::MAINMENU) {
                _mainMenu->windowResized();
            }
        }

        if (_gameState == GameState::MAINGAME) {
            _mainGame->gameLoop();
        }
        else if (_gameState == GameState::MAINMENU) {
            _mainMenu->gameLoop();
        }

        GlobalContext::window.swapBuffer();
        Jauntlet::Time::endFrame();
    }
}

void SceneManager::switchScene(GameState newState) {
    _gameState = newState;

    // Toggle state of MainGame
    if (_gameState == GameState::MAINGAME) {
        if (_mainGame == nullptr) {
                _mainGame = new MainGame();
        }
    } else if (_mainGame != nullptr) {
            delete _mainGame;
    }

    if (_gameState == GameState::MAINMENU) {
        if (_mainMenu == nullptr) {
            _mainMenu = new MainMenu(this);
        }
    }
    else if (_mainMenu != nullptr) {
        delete _mainMenu;
    }
}