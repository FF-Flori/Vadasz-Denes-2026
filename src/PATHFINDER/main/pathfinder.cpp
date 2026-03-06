#include "pathfinder.hpp"
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>
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
	std::cout<<"Starting calc\n";
	groupOres();
	std::cout<<"Grouped\n";

	// testing A*
	const auto testPath = Path(2, 27);
	std::cout << "\n";
	std::cout << "start: " << static_cast<int>(testPath.startPos.x) << "; " << static_cast<int>(testPath.startPos.y) << "\n";
	std::cout << "end: " << static_cast<int>(testPath.endPos.x) << "; " << static_cast<int>(testPath.endPos.y) << "\n";
	std::cout << "coords:" << "\n";
	for (const coord_t step : testPath.path)
		std::cout << static_cast<int>(step.x) << "; " << static_cast<int>(step.y) << std::endl;
	// end testing A*

	//fill up the lookup table
	/* A bit of explanation:
	 * Let's say for example that oreGroups.size() = 4
	 *    [0][1][2][3]
	 * [0]    .  .  .
	 * [1]       .  .
	 * [2]          .
	 * [3]
	 * This is how a 2D lookup table will look with the unnescecary elements removed.
	 * As you can see there are a lot of unused spaces. We will shrink this down into a 1D array, and not store the empty spaces
	 * This will require a bit more math on the indexing size, but that's not that big of a deal
	 * The amount of elements in this array, will be the sum of all integers from 1, up to oreGroups.size()-1
	 * (because in the 2nd row there is 1 element, in the first there are 2 and so on)
	 * We can get the sum of all integers from one up to a value (both ends included) with this formula:
	 * S = (amount of elements to be summed)(startval+endval)/2
	 * This can be simplified because we know the start val is always 1:
	 * S = endval*(endval+1)/2        where endval is ofcourse oreGroups.size()
	 */
	std::cout<<"Calculating paths..\n";
	paths.reserve(oreGroups.size()*(oreGroups.size()+1)/2);
	for(uint16_t a = 0; a < oreGroups.size(); a++){
		for(uint16_t b = a+1; b < oreGroups.size();b++)
		{ paths.push_back(Path(a,b)); }
	}
	paths.shrink_to_fit();
	std::cout<<"Starting genetic algorithm...\n";
	// start Genetic
	// 100 is temporary
	GeneticAlgorithm(100);
}

Pathfinder::OreGroup::OreGroup(const tile_t ore, const uint8_t oreValue) : ore(ore), oreValue(oreValue) {
	tiles.reserve(MAP_WIDTH);
}

Pathfinder::Path::Path(const size_t a, const size_t b) : groupA(a), groupB(b) {
	path.reserve(MAP_WIDTH);
	getClosestTiles();
	aStar();
}

void Pathfinder::Path::getClosestTiles() {
	uint8_t minDistance = -1;
	uint16_t minDiagonal = -1;
	// ReSharper disable once CppDFANullDereference
	for (const coord_t a : pathfinder->oreGroups[groupA].tiles) {
		for (const coord_t b : pathfinder->oreGroups[groupB].tiles) {
			if (const uint8_t distance = getChebyshev(a, b); distance == minDistance) {
				if (const uint16_t diagonal = getSquaredDiagonal(a, b); diagonal < minDiagonal) {
					minDistance = distance;
					minDiagonal = diagonal;
					startPos = a;
					endPos = b;
				}
			} else if (distance < minDistance) {
				minDistance = distance;
				minDiagonal = getSquaredDiagonal(a, b);
				startPos = a;
				endPos = b;
			}
		}
	}
}

void Pathfinder::Path::aStar() {
	struct TraceData {
		direction_t parent;
		uint16_t g;
	};

	std::priority_queue<Node, std::vector<Node>, std::greater<>> openSet;
	std::unordered_map<coord_t, TraceData, CoordHash> tracesToStart;
	tracesToStart.reserve(3*MAP_WIDTH);

	openSet.emplace(startPos, 0, endPos);
	tracesToStart[startPos] = {Directions::NODIRECTION, 0};

	bool searching = true;
	while (searching) {
		// no path found :(
		if (openSet.empty()) [[unlikely]] {
			return;
		}
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
				tracesToStart[endPos] = {-direction, 67};
				searching = false;
				break;
			}
			// if wall
			// ReSharper disable once CppDFANullDereference
			if (pathfinder->map[getIndex(neighbor)] == tile_t::wall) {
				continue;
			}
			uint16_t newG = node.g + 1;
			// if already mapped
			if (const auto neighborData = tracesToStart.find(neighbor); neighborData != tracesToStart.end()) {
				// if deprecated
				if (tracesToStart[node.coords].parent != node.parent) {
					continue;
				}
				// if better path found
				if (neighborData->second.g > newG) {
					neighborData->second.g = newG;
					neighborData->second.parent = -direction;
					openSet.emplace(neighbor, newG, endPos, -direction);
				}
				continue;
			}
			openSet.emplace(neighbor, newG, endPos, -direction);
			tracesToStart[neighbor] = {-direction, newG}; // flipped direction vector with operator sigma
		}
	}
	path.push_back(endPos);
	while (path.back() != startPos) {
		path.push_back(path.back() + tracesToStart[path.back()].parent);
	}
	path.shrink_to_fit();
}

void Pathfinder::groupOres() {
	std::cout<<"Called groupOres\n";
	oreGroups.reserve(65);
	for(uint8_t y = 0; y < MAP_WIDTH; y++){
		for(uint8_t x = 0; x < MAP_WIDTH; x++){
			tile_t value = map[getIndex({x,y})];
			if(value == tile_t::yellow || value == tile_t::green || value == tile_t::blue){
				createGroup(x, y);
			}
		}
	}
	createGroup(startPos.x, startPos.y);
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
				std::cout<<"ERROR\n";
				continue;
		}
		for(coord_t tile : group.tiles){
			std::cout<<"{"<<static_cast<int>(tile.x)<<","<<static_cast<int>(tile.y)<<"},";
		}
		std::cout<<"\n";
	}
	std::cout<<"There are "<<oreGroups.size()<<"groups\n";
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

void Pathfinder::GeneticAlgorithm(const uint64_t duration){
	uint16_t pathsSize = oreGroups.size()-1;
	std::array<genome_t,genomeNum> genomes;
	std::cout<<"Generating paths...\n";
	for(int i = 0; i < genomeNum; ++i){
		std::cout<<i<<" ";
		genomes.at(i).generated.reserve(pathsSize);
		genomes.at(i).score = 0;
		generatePath(genomes.at(i).generated);
		std::cout<<i<<": ";
		for(int j = 0; j < pathsSize/3;j++){
			std::cout<<j;
		}
		std::cout<<"...\n";
	}
	std::cout<<"Paths generated\n";

	std::cout<<"Starting generations\n";
	for(int i = 0; i < ITERCOUNT; ++i){
		if(i>0) [[likely]]{
			//Generate new paths
		}
		for(int j = 0; j < genomeNum; j++){
			std::cout<<"-----------------\nGenome number: "<<j<<"\n";
			uint64_t usedTime = j;
			uint32_t gatheredOreValue = 0;
			uint16_t groupCount = 0; // stores the amount of groups the rover had time to go to
			simulate(genomes.at(j).generated,duration,&usedTime,&gatheredOreValue,&groupCount);
			std::cout<<"Simulatedvals:\n"<<"usedTime: "<<usedTime<<"\ngatheredOreValue: "<<gatheredOreValue<<"\ngroupCount: "<<groupCount<<"\n";
			genomes.at(j).score = fitnessFunction(usedTime,gatheredOreValue,groupCount);
			std::cout<<"Fitnessscore: "<<genomes.at(j).score<<"\n";
		}
		std::cout<<"Fitnessscores:\n{";
		for(auto gnome : genomes)
		{ std::cout<<gnome.score<<", "; }
		std::cout<<"}\n";
		//Sorting
		std::sort(genomes.begin(),genomes.end(),[](const genome_t a, const genome_t b){return a.score < b.score;});
		std::cout<<"Fitnessscores:\n{";
	}
	return;
}
void Pathfinder::simulate(std::vector<uint16_t> path, const uint64_t duration,uint64_t *usedTime,uint32_t *gateredOreValue,uint16_t*groupCount){
	uint16_t posinpath = 0; // A bit of bad naming, but this isnt the position in the path variable, its a position in the path between two nodes
	uint16_t visitingIndex = 0; // This is the position in the path var
	Path *currpath = nullptr; // The index of the path currently traversing
	coord_t currpos = {0,0};
	// oreGroups.size is ofcourse the length of the path var
	while(visitingIndex < oreGroups.size()){
		posinpath++;
		if(posinpath > 25){
			visitingIndex++;
			posinpath = 0;
		}
	}
}
void Pathfinder::generatePath(std::vector<uint16_t> path){for(uint16_t i = 0; i < oreGroups.size();i++){path.push_back(i);}}
uint32_t Pathfinder::fitnessFunction(uint64_t usedTime,uint32_t gateredOreValue,uint16_t groupCount){return usedTime;}
