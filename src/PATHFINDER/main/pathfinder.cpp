#include "pathfinder.hpp"
#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <stdexcept>

Pathfinder::Pathfinder(const uint16_t timeLimit, const std::string& mapPath) : timeLimit(timeLimit) {
	// try to open file
	std::ifstream file(mapPath);
	if (!file.is_open()) {
		throw std::runtime_error("Couldn't open file: " + mapPath);
	}

	std::string line;
	uint8_t y = 0;

	// read lines
	while (std::getline(file, line) && y < MAP_WIDTH) {
		// read chars
		for (uint8_t x = 0; x < MAP_WIDTH*2 && x < line.length(); x+=2) {
			char c = line[x];

			tile_t type;
			switch (c) {
				case '.': type = tile_t::land; break;
				case '#': type = tile_t::wall; break;
				case 'B': type = tile_t::blue; break;
				case 'G': type = tile_t::green; break;
				case 'Y': type = tile_t::yellow; break;
				case 'S': type = tile_t::start;
					startPos.x = x; startPos.y = y;
					break;
				default:
					std::cout << "Unknown character found while reading map file!\n";
					type = tile_t::wall;
					break;
			}

			// append value to map
			map[getIndex(x, y)] = type;
		}
		y++;
	}

	file.close();

	groupOres();
}

Pathfinder::OreGroup::OreGroup(const tile_t ore, const uint8_t oreValue) : ore(ore), oreValue(oreValue) {
	tiles.reserve(60);
}

void Pathfinder::groupOres() {

}

void Pathfinder::createGroup(const uint8_t x, const uint8_t y){
	OreGroup newGroup = OreGroup(map.at(x+y*MAP_WIDTH));
	newGroup.tiles.push_back({x,y});

	int counter = 0;
	while(newGroup.tiles.size() > counter && newGroup.tiles.size() <=9){

		coord_t tileposition = newGroup.tiles.at(counter);
		//left
		if(map.at(tileposition[0]-1+tileposition[1]*MAP_WIDTH) == newGroup.ore){}


		counter++;
	}
}
