#include "pathfinder.hpp"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <queue>
#include <random>
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

	std::cout<<"Calculating paths..\n";
	paths.reserve(oreGroups.size() * (oreGroups.size() + 1) / 2);
	for(size_t a = 0; a < oreGroups.size(); a++){
		for(size_t b = a + 1; b < oreGroups.size(); b++) {
			paths.emplace_back(a, b);
		}
	}
	paths.shrink_to_fit();
	std::cout<<"Starting genetic algorithm...\n";
	// start Genetic
	GeneticAlgorithm();
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

	for(size_t i = 0; i < newGroup.tiles.size() && newGroup.tiles.size() <= GROUP_LIMIT; i++){
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

void Pathfinder::GeneticAlgorithm() const {
	// oreGroups size without start tile
	const uint16_t dnaSize = oreGroups.size() - 1;

	// fill up a template vector for the dna fields
	std::vector<uint16_t> dnaOrder(dnaSize);
	std::iota(dnaOrder.begin(), dnaOrder.end(), 0);

	Genome::initDistribution(dnaSize);

	std::cout<<"Generating random genomes...\n";
	std::vector<Genome> generation(GENERATION_SIZE);
	std::vector<Genome> oldGeneration(GENERATION_SIZE);
	for (uint16_t i = 0; i < GENERATION_SIZE; i++) {
		// give a random dna to each genome
		generation.emplace_back();
		generation.back().dna = dnaOrder;
		std::shuffle(generation.back().dna.begin(), generation.back().dna.end(), gen);
		generation.back().getFitness();

		// runtime log
		std::cout << i << ": ";
		for (int j = 0; j < dnaSize / 3; j++) {
			std::cout << j;
		}
		std::cout << "...\n";
	}
	std::cout << "Genomes generated\n";

	// sort the elements for the next generation
	std::partial_sort(generation.begin(), generation.begin() + ELITISM, generation.end(), std::greater<>());

	std::cout << "Starting generations\n";
	for (uint16_t generationIndex = 0; generationIndex < GENETIC_ITERS; generationIndex++) {
		oldGeneration = generation;

		// elitism
		generation.resize(ELITISM);

		uint16_t i = 0;

		// breeding
		for (; i < BREEDING_MUTATION; i++) {
			// tournament selection of 2 parents
			const uint16_t fatherIndex = tournamentSelect(oldGeneration);
			Genome& father = oldGeneration[fatherIndex];
			Genome& mother = oldGeneration[tournamentSelect(oldGeneration, fatherIndex)];
			generation.emplace_back(father + mother);

			if (i < BREEDING_SWAP) {
				generation.back().swap();
			} else if (i < BREEDING_SWAP + BREEDING_SCRAMBLE) {
				generation.back().scramble();
			} else if (i < BREEDING_SWAP + BREEDING_SCRAMBLE + BREEDING_INSERTION) {
				generation.back().insertion();
			} else {
				generation.back().inversion();
			}

			generation.back().getFitness();
		}
		for (; i < BREEDING; i++) {
			// tournament selection of 2 parents
			const uint16_t fatherIndex = tournamentSelect(oldGeneration);
			Genome& father = oldGeneration[fatherIndex];
			Genome& mother = oldGeneration[tournamentSelect(oldGeneration, fatherIndex)];
			generation.emplace_back(father + mother);

			generation.back().getFitness();
		}

		// cloning
		for (; i < BREEDING + CLONING; i++) {
			generation.emplace_back(oldGeneration[tournamentSelect(oldGeneration)]);

			if (i < BREEDING + CLONING_SWAP) {
				generation.back().swap();
			} else if (i < BREEDING + CLONING_SWAP + CLONING_SCRAMBLE) {
				generation.back().scramble();
			} else if (i < BREEDING + CLONING_SWAP + CLONING_SCRAMBLE + CLONING_INSERTION) {
				generation.back().insertion();
			} else {
				generation.back().inversion();
			}

			generation.back().getFitness();
		}

		// fill up with random new genomes
		for (; i < GENERATION_SIZE - ELITISM; i++) {
			generation.emplace_back();
			generation.back().dna = dnaOrder;
			std::shuffle(generation.back().dna.begin(), generation.back().dna.end(), gen);

			generation.back().getFitness();
		}

		// sort the created generation
		std::partial_sort(generation.begin(), generation.begin() + ELITISM, generation.end(), std::greater<>());
	}

	// TODO: simulate the best genome and save it
}

uint16_t Pathfinder::tournamentSelect(const std::vector<Genome>& generation) {
	uint16_t winner = generation_dist(gen);
	for (uint16_t i = 0; i < BREEDING_TOURNAMENT_SIZE; ++i) {
		if (const uint16_t participant = generation_dist(gen); generation[participant].score > generation[winner].score) {
			winner = participant;
		}
	}
	return winner;
}

uint16_t Pathfinder::tournamentSelect(const std::vector<Genome>& generation, const uint16_t unwantedParticipant) {
	uint16_t winner = generation_dist(gen);
	for (uint16_t i = 0; i < BREEDING_TOURNAMENT_SIZE; ++i) {
		uint16_t participant = 0;

		// try to exclude unwanted participant (ideally other parent)
		for (uint8_t r = 0; r < INDEX_COLLISION_RETRIES; ++r) {
			participant = generation_dist(gen);
			if (participant != unwantedParticipant) [[likely]] {break;}
		}

		if (generation[participant].score > generation[winner].score) {
			winner = participant;
		}
	}
	return winner;
}

Pathfinder::Genome Pathfinder::Genome::operator+(const Genome& other) const {
	Genome child;
	child.dna.resize(dna.size());

	// copy random dna segment from first parent
	const uint16_t a = index_dist(gen);
	const uint16_t b = index_dist(gen);
	const uint16_t segmentMin = min(a, b);
	const uint16_t segmentMax = max(a, b);
	std::vector<bool> childDnaFilled(dna.size());

	for (uint16_t i = segmentMin; i < segmentMax; i++) {
		child.dna[i] = dna[i];
		childDnaFilled[dna[i]] = true;
	}

	// complete child's dna with the remaining dna segments from the second parent
	uint16_t i = 0;
	for (const uint16_t gene : other.dna) {
		if (!childDnaFilled[gene]) {
			if (i == segmentMin) [[unlikely]] {
				i = segmentMax;
			}
			child.dna[i++] = gene;
		}
	}

	return child;
}

void Pathfinder::Genome::swap() {
	const uint16_t a = index_dist(gen);
	uint16_t b = 0;
	for (uint8_t r = 0; r < INDEX_COLLISION_RETRIES; ++r) {
		b = index_dist(gen);
		if (a != b) [[likely]] {break;}
	}

	std::swap(dna[a], dna[b]);
}

void Pathfinder::Genome::scramble() {
	const uint16_t a = index_dist(gen);
	uint16_t b = 0;
	for (uint8_t r = 0; r < INDEX_COLLISION_RETRIES; ++r) {
		b = index_dist(gen);
		if (a != b) [[likely]] {break;}
	}

	if (a < b) {
		std::shuffle(dna.begin() + a, dna.begin() + b + 1, gen);
	} else {
		std::shuffle(dna.begin() + b, dna.begin() + a + 1, gen);
	}
}

void Pathfinder::Genome::insertion() {
	const uint16_t a = index_dist(gen);
	uint16_t b = 0;
	for (uint8_t r = 0; r < INDEX_COLLISION_RETRIES; ++r) {
		b = index_dist(gen);
		if (a != b) [[likely]] {break;}
	}

	const uint16_t gene = dna[a];
	dna.erase(dna.begin() + a);
	dna.insert(dna.begin() + b, gene);
}

void Pathfinder::Genome::inversion() {
	const uint16_t a = index_dist(gen);
	uint16_t b = 0;
	for (uint8_t r = 0; r < INDEX_COLLISION_RETRIES; ++r) {
		b = index_dist(gen);
		if (a != b) [[likely]] {break;}
	}

	if (a < b) {
		std::reverse(dna.begin() + a, dna.begin() + b + 1);
	} else {
		std::reverse(dna.begin() + b, dna.begin() + a + 1);
	}
}

bool Pathfinder::calculateGroupBatteryAndTimeUsage(const OreGroup* pOreGroup, uint8_t &startBattery, uint64_t &startTime) const {
	// This is a gross overestimation
	const uint8_t estimatedMoves = pOreGroup->tiles.size()*2;

	constexpr int8_t energyusage[2*2] = {
		// day, night
		9, -1,		// 10-idle(1), -idle
		8, -2,		// 10-2*1^2, -2*1^2
	};

	if(startTime%24 > 20){
		// The only two ways to survive the night are:
		// The rover has enough energy to idle through it
		// The rover can mine all the ores before dawn, or before it runs out of energy
		// (Both of these assume that there is enough time to finish the night)
		uint8_t timeleft = 24-startTime%24;
		uint8_t usableTime = timeLimit-startTime;
		if( (timeleft > startBattery) ||
			!(estimatedMoves < timeleft && estimatedMoves*energyusage[3] <= startBattery))
		{ startBattery = 0; }

		if(timeleft > usableTime || estimatedMoves > usableTime)
		{ startTime = 0; }

		if(!startTime || !startBattery)
		{ return false; }
	}

	int8_t costOfNight[2];
	costOfNight[0] = -4; // The energy cost of spending a single night idle
	costOfNight[1] = energyusage[3]; // The energy cost of spending a single night with 1 speed

	int posInPath = 0;
	while(posInPath < estimatedMoves){
		// Nights start at 20
		const bool isDay = startTime%24 < 21;
		const int8_t addedEnergy = energyusage[2+!isDay];

		if(isDay){
			if(startBattery+addedEnergy >= costOfNight[0] || startBattery+addedEnergy >= costOfNight[1]){
				startBattery += addedEnergy;
				posInPath++;
				if(startBattery > 100){startBattery=100;}
			}else{
				startBattery += energyusage[0]; //idle during the day
				assert(startBattery < 101);
			}
		}else{
			if(24-startTime%24*addedEnergy > startBattery){
				startBattery--; //idle at night
				assert(startBattery < 101);
			}else{
				startBattery+=addedEnergy;
				posInPath++;
				assert(startBattery < 101);
			}
		}

		startTime++;
		if(startTime>timeLimit){return false;}
	}

	return true;
}
// The energy usage equation: Eusage = 2*speed^2
// The rover gets 10 energy during the day
bool Pathfinder::calculateBatteryAndTimeUsage(const Path* pathToCheck, uint8_t &startBattery, uint64_t &startTime, uint8_t speed) const {
	assert(speed > 0 && speed < 4);
	// Four rows 2 columns
	constexpr int8_t energyUsage[4*2] = {
		// day, night
		9, -1,		// 10-idle(1), -idle
		8, -2,		// 10-2*1^2, -2*1^2
		2, -8,		// 10-2*2^2, -2*2^2
		-8, -18		// 10-2*3
	};
	if(startTime%24 > 20){
		if(24-startTime%24 > startBattery)
		{ startBattery = 0; return false; }
		if(24-startTime%24 + startTime > timeLimit)
		{ startTime = 0; return false; }
	}
	int8_t costofNight[2];
	costofNight[0] = -4; // The energy cost of spending a single night idle
	costofNight[1] = energyUsage[speed*2+1]; // The energy cost of spending a single night going with the specified speed

	size_t posInPath = 0;
	while(posInPath < pathToCheck->path.size()){
		// Nights start at 20
		const bool isDay = startTime%24 < 21;
		if(pathToCheck->path.size()-posInPath < speed)
		{ speed = pathToCheck->path.size()-posInPath; }

		const int8_t addedEnergy = energyUsage[speed*2+!isDay];

		if(isDay){
			if(startBattery+addedEnergy >= costofNight[0] || startBattery+addedEnergy >= costofNight[1]){
				startBattery += addedEnergy;
				posInPath+=speed;
				if(startBattery > 100){startBattery=100;}
				//It is guaranteed that this won't go below zero because costofNight is always > 0
			}else{
				startBattery += energyUsage[0]; //idle during the day
				assert(startBattery < 101);
			}
		}else{
			if(24-startTime%24*addedEnergy > startBattery){
				startBattery--; //idle at night
				assert(startBattery < 101);
			}else{
				startBattery+=addedEnergy;
				posInPath+=speed;
				assert(startBattery < 101);
			}
		}

		startTime++;
		if(startTime>timeLimit){ startTime = 0; return false;}
	}
	return true;
}
void Pathfinder::simulate(const std::vector<uint16_t> path, uint64_t* usedTime, uint32_t* gateredOreValue, uint16_t* groupCount){
	uint64_t timeused = 0;
	uint16_t posinpath = 0;
	uint8_t battery = 100;

	// path to each group + the usages of the group itself + the path back
	state_t states[path.size()*2+1];
	uint16_t lastGroup = oreGroups.size()-1;

	for(int i = 0; i < path.size(); i++){
		uint8_t tempBattery; uint64_t tempTime;
		uint8_t speed = 3;
		for(; speed > 0;speed--){
			tempBattery = battery;
			tempTime = timeused;
			if(calculateBatteryAndTimeUsage(&paths.at(getPathIndex(path.at(i),lastGroup)),tempBattery,tempTime,3))
			{break;}
		}
		if(speed < 1){
			for(int j = i; j > -1; j--){

			}
		}else{
			battery = tempBattery;
			timeused = tempTime;
			states[i*2] = {
				.timeUsage = tempTime,
				.energyUsage = tempBattery,
				.usedSpeed = speed
			};
		}


		// Now for the group
		tempBattery = battery;
		tempTime = timeused;
		if(!calculateGroupBatteryAndTimeUsage(&oreGroups.at(path.at(i)),tempBattery,tempTime)){
			// TODO: this
			// Impossible
		}else{
			battery = tempBattery;
			timeused = tempTime;
			states[i*2+1] = {
				.timeUsage = tempTime,
				.energyUsage = tempBattery,
				.usedSpeed = 1
			};
		}

		lastGroup = path.at(i);
	}
	// TODO: simulate() does not work yet
}

Pathfinder::bfsState Pathfinder::runFastBFS(const uint16_t targetDist, const uint16_t startTime, const uint8_t startBattery) const {
	static thread_local std::vector<uint16_t> memoTable;

	const uint32_t requiredSize = (targetDist + 1) * (timeLimit + 1);

	if (memoTable.size() < requiredSize) {
		memoTable.resize(requiredSize, -1);
	}

	std::fill_n(memoTable.begin(), requiredSize, -1);

	std::queue<bfsState> q;
	q.push({0, startTime, startBattery});

	memoTable[startTime] = startBattery;

	while(!q.empty()) {
		const bfsState state = q.front();
		q.pop();

		if (state.dist == targetDist) [[unlikely]] {
			return state;
		}

		const uint16_t nextTime = state.time + 1;
		if (nextTime > timeLimit) {
			continue;
		}

		const uint8_t charge = (state.time % 48 < 32) ? 10 : 0;

		for (int i = 0; i < 4; i++) {
			if (state.battery >= STEP_COST[i]) {
				const uint16_t nextDist = std::min(targetDist, static_cast<uint16_t>(state.dist + i));
				int nextBatCalc = state.battery - STEP_COST[i] + charge;
				const uint8_t nextBattery = std::min(100, nextBatCalc);

				if (const uint32_t index = nextDist * timeLimit + nextTime; memoTable[index] < nextBattery) {
					memoTable[index] = nextBattery;
					q.push({nextDist, nextTime, nextBattery});
				}
			}
		}
	}
	return {0, 0, 0};
}

uint32_t Pathfinder::fitness(const Genome* genome) const {
	uint32_t totalValue = 0;
	uint16_t time = START_TIME;
	uint8_t battery = START_BATTERY;
	uint16_t group = oreGroups.size() - 1;

	for (const uint16_t target : genome->dna) {
		const uint16_t distToTarget = paths[getPathIndex(group, target)].path.size() - 1;
		const uint16_t distToBase = paths[getPathIndex(target, oreGroups.size() - 1)].path.size() - 1;

		const bfsState toTarget = runFastBFS(distToTarget, time, battery);

		if (toTarget.time == 0 && distToTarget > 0) {
			break;
		}

		const uint16_t targetValue = oreGroups[target].tiles.size() * oreGroups[target].oreValue;
		// slightly overthrow the value to protect unusual oreGroups
		const uint16_t targetActions = oreGroups[target].tiles.size() * 22 / 10;

		uint16_t timeAfterTarget = toTarget.time;
		uint8_t batteryAfterTarget = toTarget.battery;

		for (uint16_t t = 0; t < targetActions; ++t) {
			timeAfterTarget++;
			if (timeAfterTarget % 48 < 32) {
				batteryAfterTarget = std::min(100, batteryAfterTarget + 8);
			} else {
				if (batteryAfterTarget >= 2) {
					batteryAfterTarget -= 2;
				}
			}
		}

		if (const bfsState toBase = runFastBFS(distToBase, time, battery);
		toBase.time > 0 || distToBase == 0) {
			totalValue += targetValue;
			time = timeAfterTarget;
			battery = batteryAfterTarget;
			group = target;

		} else {
			break;
		}
	}
	return totalValue;
}

/*
void Pathfinder::calculateRoute(route_t& toRoute, const Genome& genome) const {
	toRoute.instructions.clear();

	uint16_t time = START_TIME;
	uint8_t battery = START_BATTERY;
	uint32_t totalValue = 0;
	uint16_t previousGroup = oreGroups.size() - 1;

	for (size_t g = 0; g < genome.dna.size(); g++) {
		const uint16_t targetGroup = genome.dna[g];
		uint16_t distance = paths[getPathIndex(previousGroup, targetGroup)].path.size() - 1;



		previousGroup = targetGroup;
	}


	toRoute.instructions.clear();

	// {segmentType0, battery0, segmentType1, battery1...}
	std::vector<uint8_t> routeSegments;
	routeSegments.reserve(genome.dna.size() * 6);

	uint16_t remainingPath = paths[getPathIndex(0, genome.dna[0])].path.size() - 1;
	uint16_t g = 0;                // group index in genome dna
	uint32_t i = 0;                 // segment index
	uint16_t time = START_TIME;      // time in half hours
	uint8_t battery = START_BATTERY;  // battery percentage (0-100)
	std::vector<uint32_t> wastefulSegments;
	while (time < timeLimit) {
		if (time % 48 < 32)              {battery += 10;}
		if ((time + 1) % 48 < timeLimit) {battery += 10;}

		if (battery >= segmentTypes[1][0]) {
			routeSegments.push_back(1);
			routeSegments.push_back(segmentTypes[1][0]);
			wastefulSegments.push_back(i);
			time += 2;
			remainingPath -= 6;
			i += 2;
			continue;
		}

		if ((time + 2) % 48 < timeLimit) {battery += 10;}

		if (battery >= segmentTypes[2][0]) {
			routeSegments.push_back(2);
			routeSegments.push_back(segmentTypes[2][0]);
			time += 3;
			remainingPath -= 6;
			i += 2;
			continue;
		}

		const uint8_t energyRequired = segmentTypes[2][0] - battery;

		while (!wastefulSegments.empty() && energyRequired <= 0) {
			routeSegments[wastefulSegments.back()]--;
			wastefulSegments.pop_back();
		}



		for ()

		// this can not happen
		// if (battery > 100) [[unlikely]] {
		// 	battery = 100;
		// }
	}
}
*/