#include "pathfinder.hpp"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <stdexcept>

Pathfinder::Pathfinder(const uint16_t timeLimit, const std::string& mapPath) : timeLimit(timeLimit) {
	// try to open file
	std::cout<<"Opening...\n";
	std::ifstream file(mapPath);
	if (!file.is_open()) {
		throw std::runtime_error("Couldn't open file: " + mapPath);
	}

	std::string line;
	uint8_t y = 0;

	// read lines
	while (std::getline(file, line) && y < MAP_WIDTH) {
		std::cout<<"\n"<<static_cast<int>(y)<<": ";
		// read chars
		for (uint8_t x = 0; x < MAP_WIDTH && x < line.length(); x++) {
			const char c = line[x*2];

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
			std::cout<<c;

			// append value to map
			map[getIndex(x, y)] = type;
		}
		y++;
	}
	std::cout<<"\nDone opening file\n";

	file.close();

	groupOres();
}

Pathfinder::OreGroup::OreGroup(const tile_t ore, const uint8_t oreValue) : ore(ore), oreValue(oreValue) {
	tiles.reserve(GROUP_LIMIT);
}

Pathfinder::Path::Path(const size_t a, const size_t b, Pathfinder& pathfinder) : a(a), b(b) {

	astar(pathfinder.map);
}

void Pathfinder::Path::astar(map_t& map) {

}

void Pathfinder::groupOres() {
	oreGroups.reserve(20);
	std::cout<<"Called groupOres\n";
	for(uint8_t y = 0; y < MAP_WIDTH; y++){
		for(uint8_t x = 0; x < MAP_WIDTH; x++){
			tile_t value = map[getIndex({x,y})];
			if(value == tile_t::yellow || value == tile_t::green || value == tile_t::blue){
				createGroup(x, y);
			}
		}
	}
	oreGroups.shrink_to_fit();
	for(const OreGroup& group : oreGroups){
		switch (group.ore) {
			case tile_t::yellow:
				std::cout<<"YELLOW:\n";
				break;
			case tile_t::green:
				std::cout<<"GREEN:\n";
				break;
			case tile_t::blue:
				std::cout<<"BLUE:\n";
				break;
			default:
				std::cout<<"ERROR ";
				continue;
		}
		for(coord_t tile : group.tiles){
			std::cout<<"{"<<static_cast<int>(tile.x)<<","<<static_cast<int>(tile.y)<<"},";
		}
		std::cout<<"\n";
	}
	std::cout<<"There are "<<oreGroups.size()<<"groups\n";
	/* A bit of explanation:
	 * Let's say for example that oreGroups.size() = 4
	 *    [0][1][2][3]
	 * [0]    .  .  .
	 * [1]       .  .
	 * [2]          .
	 * [3]
	 * This is how a 2D lookup table will look with the unnescecary elements removed.
	 * As you can see there are a lot of unused spaces. We will shrink this down into a 1D array, and not store the empty spaces
	 * This will require a bit more math on the indexing size, but thats not that big of a deal
	 * The amount of elements in this array, will be the sum of all integers from 1, up to oreGroups.size()-1
	 * (because in the 2nd row there is 1 element, in the first there are 2 and so on)
	 * We can get the sum of all integers from one up to a value (both ends included) with this formula:
	 * S = (amount of elements to be summed)(startval+endval)/2
	 * This can be simplified because we know the start val is always 1:
	 * S = endval*(endval+1)/2        where endval is ofcourse oreGroups.size()
	 */
}

void Pathfinder::checkCoord(const uint8_t x, const uint8_t y, const tile_t oreType, OreGroup& group){
	if(x < 0 || MAP_WIDTH <= x){return;}
	if(y < 0 || MAP_WIDTH <= y){return;}
	if(group.tiles.size()>=GROUP_LIMIT){return;}

	if(map[getIndex(x,y)] == oreType) {
		group.tiles.push_back({x,y});
		map[getIndex(x,y)] = tile_t::grouped;
	}
}

void Pathfinder::createGroup(const uint8_t x, const uint8_t y){
	OreGroup newGroup = OreGroup(map[getIndex(x,y)]);
	newGroup.tiles.push_back({x,y});
	map[getIndex(x,y)] = tile_t::grouped;

	for(int i = 0; i < newGroup.tiles.size() && newGroup.tiles.size() <= GROUP_LIMIT; i++){
		coord_t tilePos = newGroup.tiles.at(i);

		//left
		checkCoord(tilePos.x-1, tilePos.y,   newGroup.ore, newGroup);
		//topleft
		checkCoord(tilePos.x-1, tilePos.y-1, newGroup.ore, newGroup);
		//top
		checkCoord(tilePos.x,   tilePos.y-1, newGroup.ore, newGroup);
		//topright
		checkCoord(tilePos.x+1, tilePos.y-1, newGroup.ore, newGroup);
		//right
		checkCoord(tilePos.x+1, tilePos.y,   newGroup.ore, newGroup);
		//bottomright
		checkCoord(tilePos.x+1, tilePos.y+1, newGroup.ore, newGroup);
		//bottom
		checkCoord(tilePos.x,   tilePos.y+1, newGroup.ore, newGroup);
		//bottomleft
		checkCoord(tilePos.x-1, tilePos.y+1, newGroup.ore, newGroup);
	}
	newGroup.tiles.shrink_to_fit();
	oreGroups.push_back(newGroup);
}
