#include "pathfinder.hpp"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <array>
#include <bitset>
#include <queue>
#include <string>
#include <stdexcept>
#include <unordered_map>

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
}

void Pathfinder::calculate() {
	groupOres();
}

Pathfinder::OreGroup::OreGroup(const tile_t ore, const uint8_t oreValue) : ore(ore), oreValue(oreValue) {
	tiles.reserve(MAP_WIDTH);
}

Pathfinder::Path::Path(const size_t a, const size_t b) : groupA(a), groupB(b) {
	if (pathfinder == nullptr) [[unlikely]] {
		throw std::runtime_error("Pathfinder instance is null! WTF?!");
	}
	path.reserve(MAP_WIDTH);
	getClosestTiles();
	aStar();
}

void Pathfinder::Path::getClosestTiles() {
	uint8_t minDistance = -1;
	// ReSharper disable once CppDFANullDereference
	for (const coord_t a : pathfinder->oreGroups[groupA].tiles) {
		for (const coord_t b : pathfinder->oreGroups[groupB].tiles) {
			if (const uint8_t distance = getChebyshev(a, b); distance < minDistance) {
				minDistance = distance;
				startPos = a;
				endPos = b;
			}
		}
	}
}

void Pathfinder::Path::aStar() {
	std::vector<Node> openSetContainer;
	openSetContainer.reserve(2*MAP_WIDTH);
	std::priority_queue<Node, std::vector<Node>, std::greater<>> openSet;
	std::unordered_map<coord_t, direction_t, CoordHash> tracesToStart;
	tracesToStart.reserve(3*MAP_WIDTH);

	openSet.emplace(startPos, 0, endPos);
	tracesToStart[startPos] = Directions::NODIRECTION;

	bool searching = true;
	while (searching) {
		// get the max f cost node
		const Node node = openSet.top();
		openSet.pop();

		for (direction_t direction : Directions::ALL) {
			// if it's their parent node
			if (direction == node.parent) {
				continue;
			}

			// if out of map
			if (node.coords < direction) [[unlikely]] {
				continue;
			}

			const coord_t neighbor = node.coords + direction;
			// if the next node is the finish
			if (neighbor == endPos) [[unlikely]] {
				tracesToStart[endPos] = -direction;
				searching = false;
				break;
			}
			// if wall or already mapped coords
			// ReSharper disable once CppDFANullDereference
			if (pathfinder->map[getIndex(neighbor)] == tile_t::wall || tracesToStart.count(neighbor)) {
				continue;
			}
			openSet.emplace(neighbor, node.g + 1, endPos, -direction);
			tracesToStart[neighbor] = -direction; // flipped direction vector with operator sigma
		}
	}
	path.push_back(endPos);
	while (path.back() != startPos) {
		path.push_back(path.back() + tracesToStart[path.back()]);
	}
	path.shrink_to_fit();
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
