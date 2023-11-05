#include "TileHandler.h"
#include <fstream>
#include <iostream>
#include <Jauntlet/Errors.h>
#include <Jauntlet/JMath.h>
#include <Jauntlet/Externals/tinyfiledialogs.h>
#include <sstream>


TileHandler::TileHandler() {
}

void TileHandler::loadFile() {
	char const* filter[1] = { "*.JML" };
	char const* filePath = tinyfd_openFileDialog("Select file to open", NULL, 1, filter, "Jauntlet Map Loader File (JML)", 1);
	// the user cancelled the selection
	if (filePath == NULL) {
		return;
	}

	std::vector<std::string> files = JMath::Split(filePath, '|');

	// remove empty string if only one file is selected
	if (files.back().empty()) {
		files.pop_back();
	}

	// We have to reserve space in the tilemap vector because if the vector needs more space, it will allocate more space which requires the vector to need to be moved.
	// this causes the existing tilemaps to get deleted which then of course causes errors.
	_tileMaps.reserve(sizeof(Jauntlet::TileMap) * files.size());

	std::ifstream file;
	for (int i = 0; i < files.size(); i++) {
		file.open(files[i]);

		if (file.fail()) {
			std::cout << "Failed to open TileMap " << files[i] << std::endl;
			file.close();
			return;
		}

		_tileMapfilePath.push_back(files[i]);
		_tileMaps.emplace_back(_textureCache, 64);
		_levelInfos.push_back(std::vector<std::vector<unsigned int>>());
		_selectedTileMap = _tileMaps.size() - 1;
		_tileInfo.push_back(std::vector<std::string>());

		std::string line;
		while (std::getline(file, line, '\n')) {
			if (line == "ENDDEC") {
				break;
			}
			if (JMath::Split(line, ' ')[0] == "offset") {
				continue;
			}

			_tileInfo[i].push_back(line);
		}

		int y = 0;
		_levelInfos[_selectedTileMap].push_back(std::vector<unsigned int>());
		while (std::getline(file, line, '\n')) {
			std::stringstream ss(line);
			while (std::getline(ss, line, ',')) {
				_levelInfos[_selectedTileMap][y].push_back(stoi(line));
			}
			y++;
			_levelInfos[_selectedTileMap].push_back(std::vector<unsigned int>());
		}

		file.close();

		_tileMaps[_selectedTileMap].loadTileMap(files[i], 0, 0); // currently we do not use offset. -xm
	}

	// bit of a hack, but we change the color scale of all tilemaps this way. -xm
	changeSelectedTileMap(0);
}
void TileHandler::saveAllFiles() {
	// start by cleaning the tilemaps
	cleanTileMaps();
	
	std::ofstream file;

	for (int i = 0; i < _tileMapfilePath.size(); i++) {
		file.open(_tileMapfilePath[i]);

		// add all tiles to the file
		for (int j = 0; j < _tileInfo[i].size(); j++) {
			file << _tileInfo[i][j] << std::endl;
		}
		file << "ENDDEC" << std::endl;

		for (int y = 0; y < _levelInfos[i].size(); y++) {
			for (int x = 0; x < _levelInfos[i][y].size(); x++) {
				if (x + 1 == _levelInfos[i][y].size()) {
					file << _levelInfos[i][y][x] << std::endl;
				}
				else {
					file << _levelInfos[i][y][x] << ",";
				}
			}
		}

		file.close();
	}
}
void TileHandler::saveSelectedTileMapAs() {
	cleanTileMaps();

	char const* filter[1] = { "*.JML" };
	char const* filePath = tinyfd_saveFileDialog("Select where to save file", 
	NULL, 1, filter, "Jauntlet Map Loader (JML)");
	// the user cancelled the selection
	if (filePath == NULL) {
		return;
	}

	std::ofstream file(filePath);

	for (int i = 0; i < _tileInfo[_selectedTileMap].size(); i++) {
		file << _tileInfo[_selectedTileMap][i] << std::endl;
	}
	file << "ENDDEC" << std::endl;

	for (int y = 0; y < _levelInfos[_selectedTileMap].size(); y++) {
		for (int x = 0; x < _levelInfos[_selectedTileMap][y].size(); x++) {
			if (x + 1 == _levelInfos[_selectedTileMap][y].size()) {
				file << _levelInfos[_selectedTileMap][y][x] << std::endl;
			}
			else {
				file << _levelInfos[_selectedTileMap][y][x] << ",";
			}
		}
	}
}

void TileHandler::draw() {
	for (int i = 0; i < _tileMaps.size(); i++) {
		_tileMaps[i].draw();
	}
}
void TileHandler::setSelectedTile(unsigned int index) {
	 (index > _tileInfo[_selectedTileMap].size())? _selectedTileID = _tileInfo[_selectedTileMap].size(): _selectedTileID = index;
}
void TileHandler::changeSelectedTile(int changeAmount) {
	// this could be changed to a while statement if we were expecting changes of more than one. -xm
	changeAmount = std::max(-1, std::min(changeAmount, 1));

	if (_selectedTileID == 0 && changeAmount == -1) {
		_selectedTileID = _tileInfo[_selectedTileMap].size();
		return;
	}
	if (_selectedTileID == _tileInfo[_selectedTileMap].size() && changeAmount == 1) {
		_selectedTileID = 0;
		return;
	}
	_selectedTileID += changeAmount;
}
void TileHandler::changeSelectedTileMap(int changeAmount) {
	// this could be changed to a while statement if we were expecting changes of more than one. -xm
	changeAmount = std::max(-1, std::min(changeAmount, 1));

	if (_selectedTileMap == 0 && changeAmount == -1) {
		return;
	}
	if (_selectedTileMap == _tileMaps.size() - 1 && changeAmount == 1) {
		return;
	}
	_selectedTileMap += changeAmount;
	_shadeUnselectedMaps = true;

	for (int i = 0; i < _tileMaps.size(); i++) {
		if (i == _selectedTileMap) {
			_tileMaps[i].changeDrawColor({ 255, 255, 255 });
		}
		else {
			_tileMaps[i].changeDrawColor({ 50, 50, 50 });
		}
	}
	// correct selected tile if tilemap has smaller amount of tiles to select from
	if (_selectedTileID > _tileInfo[_selectedTileMap].size() - 1) {
		_selectedTileID = _tileInfo[_selectedTileMap].size() - 1;
	}
}
std::string TileHandler::getSelectedTileTexture() {
	if (_selectedTileID == 0) {
		return "Textures/WhiteSquare.png";
	}
	return JMath::Split(_tileInfo[_selectedTileMap][_selectedTileID - 1], '"')[1];
}
void TileHandler::toggleShadedTileMapView() {
	_shadeUnselectedMaps = !_shadeUnselectedMaps;

	for (int i = 0; i < _tileMaps.size(); i++) {
		if (i != _selectedTileMap) {
			if (_shadeUnselectedMaps) {
				_tileMaps[i].changeDrawColor({ 50, 50, 50 });
			}
			else {
				_tileMaps[i].changeDrawColor({ 255, 255, 255 });
			}
		}
	}
}

void TileHandler::resetTileMaps() {
	cleanTileMaps();
	for (int i = 0; i < _tileMaps.size(); i++) {
		_tileMaps[i].resetOffset();
	}
	forceUpdateTileMaps();
}

void TileHandler::updateTile(glm::ivec2 position) {
	if (position.x < 0) {
		shiftX(std::abs(position.x));
		position.x = 0;
	}
	if (position.y < 0) {
		shiftY(std::abs(position.y));
		position.y = 0;
	}
	
	if (_tileMaps[_selectedTileMap].getTileID(position) != _selectedTileID) {

		while (position.y >= _levelInfos[_selectedTileMap].size() - 1) {
			_levelInfos[_selectedTileMap].push_back(std::vector<unsigned int>());
			_levelInfos[_selectedTileMap][_levelInfos[_selectedTileMap].size() - 1].push_back(0);
		}

		while (position.x >= _levelInfos[_selectedTileMap][position.y].size()) {
			_levelInfos[_selectedTileMap][position.y].push_back(0);
		}

		_levelInfos[_selectedTileMap][position.y][position.x] = _selectedTileID;
		_tileMaps[_selectedTileMap].UpdateTile(position, _selectedTileID);
	}
}
void TileHandler::updateTile(glm::ivec2 position, unsigned int newID) {
	if (position.x < 0) {
		shiftX(std::abs(position.x));
		position.x = 0;
	}
	if (position.y < 0) {
		shiftY(std::abs(position.y));
		position.y = 0;
	}

	if (_tileMaps[_selectedTileMap].getTileID(position) != newID) {

		while (position.y >= _levelInfos[_selectedTileMap].size() - 1) {
			_levelInfos[_selectedTileMap].push_back(std::vector<unsigned int>());
			_levelInfos[_selectedTileMap][_levelInfos[_selectedTileMap].size() - 1].push_back(0);
		}

		while (position.x >= _levelInfos[_selectedTileMap][position.y].size()) {
			_levelInfos[_selectedTileMap][position.y].push_back(0);
		}

		_levelInfos[_selectedTileMap][position.y][position.x] = newID;
		_tileMaps[_selectedTileMap].UpdateTile(position, newID);
	}
}

glm::ivec2 TileHandler::WorldPosToTilePos(glm::vec2 position) {
	return _tileMaps[_selectedTileMap].WorldPosToTilePos(position);
}

glm::vec2 TileHandler::RoundWorldPos(glm::vec2 position) {
	return _tileMaps[_selectedTileMap].RoundWorldPos(position);
}

void TileHandler::shiftX(unsigned int amount) {
	_tileMaps[_selectedTileMap].AddOffset(glm::vec2(-64 * (int)amount, 0));

	for (int y = 0; y < _levelInfos[_selectedTileMap].size(); y++) {
		for (int i = 0; i < amount; i++) {
			_levelInfos[_selectedTileMap][y].insert(_levelInfos[_selectedTileMap][y].begin(), 0);
		}
	}
	forceUpdateTileMap();
}
void TileHandler::shiftY(unsigned int amount) {
	_tileMaps[_selectedTileMap].AddOffset(glm::vec2(0, 64 * amount));

	for (; amount > 0; amount--) {
		_levelInfos[_selectedTileMap].insert(_levelInfos[_selectedTileMap].begin(), std::vector<unsigned int>(0));
	}

	// we make i-nth element the same size as the next i-nth element so that when we update the tilemap,
	// we overwrite old data. This is cleaned up later during saving -xm
	for (int y = _levelInfos[_selectedTileMap].size() - 2; y >= 0; y--) {
		while (_levelInfos[_selectedTileMap][y].size() < _levelInfos[_selectedTileMap][y + 1].size()) {
			_levelInfos[_selectedTileMap][y].push_back(0);
		}
	}
	forceUpdateTileMap();
}

void TileHandler::forceUpdateTileMap() {
	for (int y = 0; y < _levelInfos[_selectedTileMap].size(); y++) {
		for (int x = 0; x < _levelInfos[_selectedTileMap][y].size(); x++) {
			_tileMaps[_selectedTileMap].UpdateTile(glm::vec2(x, y), _levelInfos[_selectedTileMap][y][x]);
		}
	}
}
void TileHandler::forceUpdateTileMap(unsigned int mapIndex) {
	for (int y = 0; y < _levelInfos[mapIndex].size(); y++) {
		for (int x = 0; x < _levelInfos[mapIndex][y].size(); x++) {
			_tileMaps[mapIndex].UpdateTile(glm::vec2(x, y), _levelInfos[mapIndex][y][x]);
		}
	}
}
void TileHandler::forceUpdateTileMaps() {
	for (int i = 0; i < _levelInfos.size(); i++) {
		for (int y = 0; y < _levelInfos[i].size(); y++) {
			for (int x = 0; x < _levelInfos[i][y].size(); x++) {
				_tileMaps[i].UpdateTile(glm::vec2(x, y), _levelInfos[i][y][x]);
			}
		}
	}
}

void TileHandler::cleanTileMaps() {
	for (int i = 0; i < _tileMaps.size(); i++) {
		// check beginning of X axis
		int clean = true;
		while (clean) {
			for (int y = 0; y < _levelInfos[i].size(); y++) {
				if (_levelInfos[i][y].size() > 0 && _levelInfos[i][y][0] != 0) {
					clean = false;
					break;
				}
			}
			if (clean) {
				_tileMaps[i].AddOffset(glm::vec2(64, 0));
				for (int y = 0; y < _levelInfos[i].size(); y++) {
					if (_levelInfos[i][y].size() > 0) {
						_levelInfos[i][y].erase(_levelInfos[i][y].begin());
					}
					else {
						_levelInfos[i].erase(_levelInfos[i].begin() + y);
						if (_levelInfos[i].size() == 0) {
							return;
						}
					}
				}
			}
		}

		// check end of X axis
		// This is much simplier than the other axis's, because this data is junk data, whilst the other
		// data is padding that needs to be removed only if the entire axis is empty. -xm
		for (int y = 0; y < _levelInfos[i].size(); y++) {
			while (_levelInfos[i][y].size() > 0 && _levelInfos[i][y].back() == 0) {
				_levelInfos[i][y].pop_back();
			}
		}
		
		// check beginning of Y axis
		clean = true;
		while (clean) {
			for (int x = 0; x < _levelInfos[i][0].size(); x++) {
				if (_levelInfos[i][0][x] != 0) {
					clean = false;
					break;
				}
			}
			if (clean) {
				_levelInfos[i].erase(_levelInfos[i].begin());
			}
		}

		// check end of Y axis
		clean = true;
		while (clean) {
			for (int x = 0; x < _levelInfos[i][_levelInfos[i].size() - 1].size(); x++) {
				if (_levelInfos[i][_levelInfos[i].size() - 1][x] != 0) {
					clean = false;
					break;
				}
			}
			if (clean) {
				_levelInfos[i].pop_back();
			}
		}
	}
}