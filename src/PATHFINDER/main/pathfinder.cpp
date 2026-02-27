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
					startPos[0] = x; startPos[1] = y;
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

void Pathfinder::OreGroup::collect(coord_t start, std::vector<OreGroup>& groupContainer) {
	// start collecting ores from the start pos, recursively create new ore groups and append them to the container when the current one hits the limit
	// collection method idea:
	// direction preferences (descending): UP-LEFT, UP, UP-RIGHT, LEFT, RIGHT, DOWN-LEFT, DOWN, DOWN-RIGHT
	// begin collecting recursively from the start pos by calling this function when a new ungrouped tile is found
	// if a tile is grouped, flag them as grouped by changing the value in the map array to it's grouped type
}

void Pathfinder::groupOres() {

}
