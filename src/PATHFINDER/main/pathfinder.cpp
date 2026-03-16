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
#include <execution>

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

Pathfinder::route_t Pathfinder::calculate() {
	std::cout<<"Starting calc\n";
	groupOres();
	std::cout<<"Grouped\n";

	// if no ore groups found
	if (oreGroups.size() < 2) {
		std::cout << "No ores on map, aborting pathfind.\n";
		return route_t{}; // empty route
	}

	// get paths
	std::cout<<"Calculating paths..\n";
	paths.reserve(oreGroups.size() * (oreGroups.size() - 1) / 2);
	for(size_t a = 0; a < oreGroups.size(); a++){
		for(size_t b = a + 1; b < oreGroups.size(); b++) {
			paths.emplace_back(a, b);

			// get the longest distance between 2 groups
			if (paths.back().path.size() > maxDistPerSegment) {
				maxDistPerSegment = paths.back().path.size();
			}
		}
	}
	paths.shrink_to_fit();

	std::cout<<"Starting genetic algorithm...\n";
	// Genetic
	const Genome winner = GeneticAlgorithm();

	// Calculate route from genetic winner
	route_t route;
	calculateInstructions(&winner, route);
	std::cout << "Done" << std::endl;

	// W finally
	return route;
}

Pathfinder::OreGroup::OreGroup(const tile_t ore, const uint8_t oreValue) : ore(ore), oreValue(oreValue) {
	tiles.reserve(MAP_WIDTH);
}

Pathfinder::Path::Path(const size_t a, const size_t b) : groupA(a), groupB(b) {
	path.reserve(MAP_WIDTH);
	getClosestTiles();
	aStar();
}

Pathfinder::Path::Path(const coord_t a, const coord_t b) : groupA(-1), groupB(-1) {
	startPos = a;
	endPos = b;

	if (a == b) {
		path.push_back(a);
		return;
	}

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
	tracesToStart[startPos] = {Directions::NO_DIRECTION, 0};

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
				std::cout<<"ERROR / START POS\n";
				continue;
		}
		for(coord_t tile : group.tiles){
			std::cout<<"{"<<static_cast<int>(tile.x)<<","<<static_cast<int>(tile.y)<<"},";
		}
		std::cout<<"\n";
	}
	std::cout<<"There are "<<oreGroups.size()<<"groups\n";
}

void Pathfinder::checkCoord(const int16_t x, const int16_t y, const tile_t oreType, OreGroup& group){
	if(x < 0 || MAP_WIDTH <= x){return;}
	if(y < 0 || MAP_WIDTH <= y){return;}
	if(group.tiles.size()>=GROUP_LIMIT){return;}

	if(map[getIndex(x,y)] == oreType) {
		group.tiles.push_back({static_cast<uint8_t>(x), static_cast<uint8_t>(y)});
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
		checkCoord(static_cast<int16_t>(tilePos.x-1), tilePos.y,   newGroup.ore, newGroup);
		//topleft
		checkCoord(static_cast<int16_t>(tilePos.x-1), static_cast<int16_t>(tilePos.y-1), newGroup.ore, newGroup);
		//top
		checkCoord(tilePos.x, static_cast<int16_t>(tilePos.y-1), newGroup.ore, newGroup);
		//topright
		checkCoord(static_cast<int16_t>(tilePos.x+1), static_cast<int16_t>(tilePos.y-1), newGroup.ore, newGroup);
		//right
		checkCoord(static_cast<int16_t>(tilePos.x+1), tilePos.y,   newGroup.ore, newGroup);
		//bottomright
		checkCoord(static_cast<int16_t>(tilePos.x+1), static_cast<int16_t>(tilePos.y+1), newGroup.ore, newGroup);
		//bottom
		checkCoord(tilePos.x, static_cast<int16_t>(tilePos.y+1), newGroup.ore, newGroup);
		//bottomleft
		checkCoord(static_cast<int16_t>(tilePos.x-1), static_cast<int16_t>(tilePos.y+1), newGroup.ore, newGroup);
	}
	newGroup.tiles.shrink_to_fit();
	oreGroups.push_back(newGroup);
}

Pathfinder::Genome Pathfinder::GeneticAlgorithm() const {
	// oreGroups size without start tile
	const uint16_t dnaSize = oreGroups.size() - 1;

	// fill up a template vector for the dna fields
	std::vector<uint16_t> dnaOrder(dnaSize);
	std::iota(dnaOrder.begin(), dnaOrder.end(), 0);

	Genome::initDistribution(dnaSize);

	std::cout<<"Generating random genomes...\n";
	std::vector<Genome> generation;
	std::vector<Genome> oldGeneration;
	generation.reserve(GENERATION_SIZE);
	oldGeneration.reserve(GENERATION_SIZE);

	for (uint16_t i = 0; i < GENERATION_SIZE; i++) {
		// give a random dna to each genome
		generation.emplace_back();
		generation.back().dna = dnaOrder;
		std::shuffle(generation.back().dna.begin(), generation.back().dna.end(), gen);
	}
	std::cout << "Genomes generated\n";

	// multithreading
	std::for_each(std::execution::par, generation.begin(), generation.end(), [](Genome& g) {
		g.getFitness();
	});

	// sort the elements for the next generation
	std::partial_sort(generation.begin(), generation.begin() + ELITISM, generation.end(), std::greater<>());

	std::cout << "Starting generations" << std::endl;
	try {
		for (uint16_t generationIndex = 0; generationIndex < GENETIC_ITERS; generationIndex++) {
			std::cout << std::endl << "Generation " << static_cast<int>(generationIndex) << "/" << static_cast<int>(GENETIC_ITERS) << std::endl;

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
			}
			for (; i < BREEDING; i++) {
				// tournament selection of 2 parents
				const uint16_t fatherIndex = tournamentSelect(oldGeneration);
				Genome& father = oldGeneration[fatherIndex];
				Genome& mother = oldGeneration[tournamentSelect(oldGeneration, fatherIndex)];
				generation.emplace_back(father + mother);
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
			}

			// fill up with random new genomes
			for (; i < GENERATION_SIZE - ELITISM; i++) {
				generation.emplace_back();
				generation.back().dna = dnaOrder;
				std::shuffle(generation.back().dna.begin(), generation.back().dna.end(), gen);
			}

			// multithreading
			std::for_each(std::execution::par, generation.begin() + ELITISM, generation.end(), [](Genome& g) {
				g.getFitness();
			});

			// sort the created generation
			std::partial_sort(generation.begin(), generation.begin() + ELITISM, generation.end(), std::greater<>());
		}
	} catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}

	std::cout << "Genetic algorythm finished!" << std::endl << "Best genome's groups are:" << std::endl;
	for (const uint16_t gr : generation[0].dna) {
		std::cout << gr << " (" << static_cast<int>(oreGroups[gr].tiles[0].x) << "; " << static_cast<int>(oreGroups[gr].tiles[0].y) << ")" << std::endl;
	}

	return generation[0];
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
	const uint16_t segmentMin = std::min(a, b);
	const uint16_t segmentMax = std::max(a, b);
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

uint32_t Pathfinder::fitness(const Genome* genome) const {
    uint32_t returnedValue = 0;
    uint32_t time = 0;

    for (size_t g = 0; g < genome->dna.size(); g++) {
	    // distance to next group
    	uint16_t dist = g == 0 ?
    		paths[getPathIndex(oreGroups.size() - 1, genome->dna[0])].path.size() - 1 :
    		paths[getPathIndex(genome->dna[g - 1], genome->dna[g])].path.size() - 1;

    	// estimate travel time
    	float travelTime = 0.0;
    	uint16_t currentTime = time % 48;
    	bool currentlyDay = currentTime % 48 < 32;

    	if (currentlyDay) {
    		const uint8_t currentDist = std::min(static_cast<uint16_t>(32 - currentTime), dist);
    		dist -= currentDist;
    		travelTime += static_cast<float>(currentDist) * TIME_PER_TILE_DAY;
		} else {
			const uint8_t currentDist = std::min(static_cast<uint16_t>(48 - currentTime), dist);
			dist -= currentDist;
			travelTime += static_cast<float>(currentDist) * TIME_PER_TILE_NIGHT;
    	}

    	while (dist) {
    		if (currentlyDay) {
    			const uint8_t currentDist = std::min(static_cast<uint16_t>(32), dist);
    			dist -= currentDist;
    			travelTime += static_cast<float>(currentDist) * TIME_PER_TILE_DAY;
    		} else {
    			const uint8_t currentDist = std::min(static_cast<uint16_t>(16), dist);
    			dist -= currentDist;
    			travelTime += static_cast<float>(currentDist) * TIME_PER_TILE_NIGHT;
    		}
    		currentlyDay = !currentlyDay;
    	}
    	travelTime = std::floor(travelTime);

    	// estimate mining time by slightly overthrowing for unusual groups
        const uint32_t miningTime = oreGroups[genome->dna[g]].tiles.size() * 11 / 5; // *2.2

        // estimate return time
        uint16_t returnDist = paths[getPathIndex(oreGroups.size() - 1, genome->dna[g])].path.size() - 1;
    	float returnTime = 0.0;
    	currentTime = time + static_cast<int>(travelTime) + miningTime % 48;
    	currentlyDay = currentTime % 48 < 32;

    	if (currentlyDay) {
    		const uint8_t currentDist = std::min(static_cast<uint16_t>(32 - currentTime), returnDist);
    		returnDist -= currentDist;
    		returnTime += static_cast<float>(currentDist) * TIME_PER_TILE_DAY;
    	} else {
    		const uint8_t currentDist = std::min(static_cast<uint16_t>(48 - currentTime), returnDist);
    		returnDist -= currentDist;
    		returnTime += static_cast<float>(currentDist) * TIME_PER_TILE_NIGHT;
    	}

    	while (returnDist) {
    		if (currentlyDay) {
    			const uint8_t currentDist = std::min(static_cast<uint16_t>(32), returnDist);
    			returnDist -= currentDist;
    			returnTime += static_cast<float>(currentDist) * TIME_PER_TILE_DAY;
    		} else {
    			const uint8_t currentDist = std::min(static_cast<uint16_t>(16), returnDist);
    			returnDist -= currentDist;
    			returnTime += static_cast<float>(currentDist) * TIME_PER_TILE_NIGHT;
    		}
    		currentlyDay = !currentlyDay;
    	}
    	returnTime = std::floor(returnTime);

        // if next group is possible
        if (static_cast<float>(time) + travelTime + static_cast<float>(miningTime) + returnTime <= static_cast<float>(timeLimit)) {
            // add mined ores' value and passed time
            returnedValue += oreGroups[genome->dna[g]].tiles.size() * oreGroups[genome->dna[g]].oreValue;
            time += static_cast<uint32_t>(travelTime) + miningTime;
        } else {
            break;
        }
    }

    return returnedValue;
}

void Pathfinder::calculateInstructions(const Genome* genome, route_t& toRoute) const {
	// helper tables
	std::vector<uint8_t> memoTable;    // best battery for a state
	std::vector<uint32_t> parentTable; // there is no chance, that we need an uint64_t table for this (that much

	// count of every possible bfsState (without isReturning)
	static const size_t stateSize = genome->dna.size() * maxDistPerSegment * (timeLimit + 1);

	uint16_t returnedOres = 0; // number of returned ores
	uint32_t bestStateIndex = 0; // index of the current best state
	uint16_t lastReachableGroupIndex = genome->dna.size() - 1; // last reachable group (decreases if a dead segment is found)

	// fill up helper tables with starter values
	const size_t requiredSize = stateSize * 2;
	if (requiredSize > UINT32_MAX) {
		throw std::overflow_error("calculateInstructions error: requiredSize exceeded uint32_t limits! Running this task would have taken gigabytes of memory!");
	}
	memoTable.resize(requiredSize, 0);
	parentTable.resize(requiredSize, -1);

	// get group-group and group-start distances
	std::vector<uint16_t> distances;
	distances.reserve(genome->dna.size());
	distances.push_back(paths[getPathIndex(oreGroups.size() - 1, genome->dna[0])].path.size() - 1);

	std::vector<uint16_t> returnDistances;
	returnDistances.reserve(genome->dna.size());
	returnDistances.push_back(distances[0]);

	// get paths inside groups
	std::vector<route_t> groupPaths;
	std::vector<route_t> returnGroupPaths;
	groupPaths.reserve(genome->dna.size() - 1);
	returnGroupPaths.reserve(genome->dna.size());

	// helper function to get the reading direction of a path
	auto getTileInGroup = [&](uint16_t targetGroup, uint16_t otherGroup) {
		const uint32_t pIdx = getPathIndex(targetGroup, otherGroup);
		// every path goes from higher group index to lower
		if (targetGroup > otherGroup) {
			return paths[pIdx].path.front();
		} else {
			return paths[pIdx].path.back();
		}
	};

	uint16_t baseIndex = oreGroups.size() - 1;

	for (size_t g = 0; g < genome->dna.size(); g++) {
		uint16_t currentGroup = genome->dna[g];
		uint16_t prevGroup = (g == 0) ? baseIndex : genome->dna[g - 1];

		coord_t entryTile = getTileInGroup(currentGroup, prevGroup);

		if (g + 1 < genome->dna.size()) {
			uint16_t nextGroup = genome->dna[g + 1];
			coord_t exitTile = getTileInGroup(currentGroup, nextGroup);

			groupPaths.push_back({});
			traceGroup(oreGroups[currentGroup], entryTile, exitTile, groupPaths[g]);
		}

		coord_t returnExitTile = getTileInGroup(currentGroup, baseIndex);
		returnGroupPaths.push_back({});
		traceGroup(oreGroups[currentGroup], entryTile, returnExitTile, returnGroupPaths[g]);
	}

	// bfs queue with start pos
	std::queue<bfsState> q;
	q.push({0, 0, 0, START_BATTERY});

	while(!q.empty()) {
		auto [groupIndex, dist, time, battery, isReturning] = q.front();
		q.pop();

		// throw out solved returning segments
		if (isReturning && groupIndex + 1 <= returnedOres) {
			continue;
		}

		// throw out hopeless segments
		if (groupIndex > lastReachableGroupIndex) {
			continue;
		}

		// get current state's values
		const uint16_t currentSegmentLength = isReturning ? returnDistances[groupIndex] : distances[groupIndex];
		const size_t returningOffset = isReturning ? stateSize : 0;
		const size_t thisIndex = returningOffset + (groupIndex * maxDistPerSegment + dist) * (timeLimit + 1) + time;

		// if the segment is finished
		if (dist >= currentSegmentLength) {
			// reached start
			if (isReturning) {
				returnedOres = groupIndex + 1;
				bestStateIndex = thisIndex;

				// if all groups collected
				if (returnedOres == genome->dna.size()) [[unlikely]] {break;}

			// reached an ore group - simulate the rover inside the group
			} else {
				// RETURNING BRANCH
				uint16_t targetActions = returnGroupPaths[groupIndex].size();

				// iterate over the actions in the group
				uint8_t cycleTime = time % 48;
				uint16_t elapsedTime = 0;
				uint8_t newBattery = battery;
				for (uint16_t t = 0; t < targetActions;) {
					elapsedTime++;
					cycleTime++;
					if (cycleTime >= 48) {
						cycleTime = 0;
					}
					if (cycleTime < 32) {
						newBattery = std::min(100, newBattery + 8);
					} else {
						if (newBattery >= 2) {
							newBattery -= 2;
						} else {
							newBattery = 0; // rover dies
							break;
						}
					}
					t++;
				}
				// if ran out of energy in the group
				if (newBattery == 0) [[unlikely]] {continue;}

				// if no more time left
				if (time + elapsedTime >= timeLimit) [[unlikely]] {
					// if the first group is unreachable
					if (groupIndex == 0) [[unlikely]] {
						return;
					}
					// the last reachable group is the previous one
					lastReachableGroupIndex = groupIndex - 1;
					if (lastReachableGroupIndex + 1 <= returnedOres) [[unlikely]] {break;} // if the max possible ore groups are reached
					continue;
				}

				if (groupIndex + 1 > returnedOres) {
					if (const size_t index = stateSize + groupIndex * maxDistPerSegment * (timeLimit + 1) + time + elapsedTime;
						memoTable[index] < newBattery) {
						parentTable[index] = thisIndex;
						memoTable[index] = newBattery;
						q.push({groupIndex, 0, static_cast<uint16_t>(time + elapsedTime), newBattery, true});
					}
				}

				// CONTINUING BRANCH
				if (groupIndex + 1 < static_cast<uint16_t>(genome->dna.size())) {
					targetActions = groupPaths[groupIndex].size();

					// iterate over the actions in the group
					cycleTime = time % 48;
					elapsedTime = 0;
					for (uint16_t t = 0; t < targetActions;) {
						elapsedTime++;
						cycleTime++;
						if (cycleTime >= 48) {
							cycleTime = 0;
						}
						if (cycleTime < 32) {
							battery = std::min(100, battery + 8);
						} else {
							if (battery >= 2) {
								battery -= 2;
							} else {
								battery = 0; // rover dies
								break;
							}
						}
						t++;
					}
					// if ran out of energy in the group
					if (battery == 0) [[unlikely]] {continue;}

					// if no more time left
					if (time + elapsedTime >= timeLimit) [[unlikely]] {
						// if the first group is unreachable
						if (groupIndex == 0) [[unlikely]] {
							return;
						}
						// the last reachable group is the previous one
						lastReachableGroupIndex = groupIndex - 1;
						if (lastReachableGroupIndex + 1 <= returnedOres) [[unlikely]] {break;} // if the max possible ore groups are reached
						continue;
					}

					time += elapsedTime;

					if (groupIndex < lastReachableGroupIndex) {
						if (const size_t index = groupIndex * maxDistPerSegment * (timeLimit + 1) + time;
							memoTable[index] < battery) {
							parentTable[index] = thisIndex;
							memoTable[index] = battery;
							q.push({static_cast<uint16_t>(groupIndex + 1), 0, time, battery});
						}
					}
				}
			}
			continue;
		}

		// normal movement calculation on path segment (else)
		const uint16_t nextTime = time + 1;

		// if no more time left
		if (nextTime > timeLimit) {
			// if the first group is unreachable
			if (groupIndex == 0) [[unlikely]] {
				return;
			}
			// the last reachable group is the previous one
			lastReachableGroupIndex = groupIndex - 1;
			if (lastReachableGroupIndex + 1 <= returnedOres) [[unlikely]] {break;} // if the max possible ore groups are reached
			continue;
		}

		// make next states
		if (nextTime % 48 < 32) { // daytime
			for (uint8_t i = 0; i < 4; i++) {
				// if not enough energy to complete that step (can't complete any other steps either, steps in ascending order)
				if (battery < STEP_COSTS[0][i]) {break;}
				const uint8_t nextBattery = std::min(100, battery - STEP_COSTS[0][i]);

				// if the current segment is shorter than the step (-||-)
				const uint16_t nextDist = dist + i;
				if (currentSegmentLength < nextDist) {break;}

				// calculate index in memoTable
				// if found new or better state than saved
				if (const size_t index = returningOffset + (groupIndex * maxDistPerSegment + nextDist) * (timeLimit + 1) + nextTime;
				memoTable[index] < nextBattery) {
					parentTable[index] = thisIndex;
					memoTable[index] = nextBattery;
					q.push({groupIndex, nextDist, nextTime, nextBattery, isReturning});
				}
			}

		} else { // nighttime
			for (uint8_t i = 0; i < 4; i++) {
				// if not enough energy to complete that step (can't complete any other steps either, steps in ascending order)
				if (battery < STEP_COSTS[1][i]) {break;}
				const uint8_t nextBattery = std::min(100, battery - STEP_COSTS[1][i]);

				// if the current segment is shorter than the step (-||-)
				const uint16_t nextDist = dist + i;
				if (currentSegmentLength < nextDist) {break;}

				// calculate index in memoTable
				// if found new or better state than saved
				if (const size_t index = returningOffset + (groupIndex * maxDistPerSegment + nextDist) * (timeLimit + 1) + nextTime;
				memoTable[index] < nextBattery) {
					parentTable[index] = thisIndex;
					memoTable[index] = nextBattery;
					q.push({groupIndex, nextDist, nextTime, nextBattery, isReturning});
				}
			}
		}
	}

	std::cout << "Returned ores:" << std::endl;
	std::cout << static_cast<int>(returnedOres) << std::endl;

	// trace back the best state
	if (returnedOres == 0) {
		return;
	}

	std::vector<uint32_t> parents;
	uint32_t current = bestStateIndex;

	while (current < static_cast<uint32_t>(-1) && current > 0) {
		parents.push_back(current);
		current = parentTable[current];
	}
	parents.push_back(0);

	instruction_t speed = instruction_t::set_speed_0;

	// get back state from index
	for (int64_t i = static_cast<int64_t>(parents.size()) - 2; i >= 0; i--) {
		size_t previousIndex = parents[i + 1];
		size_t currentIndex = parents[i];

		// previous state
		bool pIsReturning = previousIndex >= stateSize;
		size_t pBase = pIsReturning ? previousIndex - stateSize : previousIndex;
		size_t pDistGroup = pBase / (timeLimit + 1);
		uint16_t pDist = pDistGroup % maxDistPerSegment;
		uint16_t pGroup = pDistGroup / maxDistPerSegment;

		// current state
		bool cIsReturning = currentIndex >= stateSize;
		size_t cBase = cIsReturning ? currentIndex - stateSize : currentIndex;
		size_t cDistGroup = cBase / (timeLimit + 1);
		uint16_t cDist = cDistGroup % maxDistPerSegment;
		uint16_t cGroup = cDistGroup / maxDistPerSegment;

		// regular movement
		if (pGroup == cGroup && pIsReturning == cIsReturning) {
			uint16_t distDiff = cDist - pDist;

			// if no movement
			if (distDiff == 0) {
				if (speed != instruction_t::set_speed_0) {
					toRoute.push_back(instruction_t::set_speed_0);
					speed = instruction_t::set_speed_0;
				}
				toRoute.push_back(instruction_t::up_left);
			} else {
				// set speed mode
				if (const auto speedMode = static_cast<instruction_t>(static_cast<uint8_t>(instruction_t::set_speed_0) + distDiff); speed != speedMode) {
					toRoute.push_back(speedMode);
					speed = speedMode;
				}

				uint32_t cPathIndex;
				bool pathForward;

				if (cIsReturning) {
					// to start
					uint16_t startGroupIndex = oreGroups.size() - 1;
					cPathIndex = getPathIndex(genome->dna[cGroup], startGroupIndex);
					// start tile has the max index in groups, read forward
					pathForward = false;
				} else {
					// to next group
					uint16_t pSegmentGroup = (cGroup == 0) ? (oreGroups.size() - 1) : genome->dna[cGroup - 1];
					cPathIndex = getPathIndex(genome->dna[cGroup], pSegmentGroup);
					// direction to read path
					pathForward = pSegmentGroup > genome->dna[cGroup];
				}

				const auto& currentPath = paths[cPathIndex].path;
				size_t L = currentPath.size();

				// calculate steps
				for (uint16_t s = distDiff; s > 0; s--) {
					if (pathForward) {
						// read forward
						toRoute.push_back(currentPath[cDist - s].getInstructionTo(currentPath[cDist - s + 1]));
					} else {
						// read backward
						size_t fromIndex = L - 1 - (cDist - s);
						size_t toIndex   = L - 1 - (cDist - s + 1);
						toRoute.push_back(currentPath[fromIndex].getInstructionTo(currentPath[toIndex]));
					}
				}
			}
			// in group movement
		} else {
			speed = instruction_t::set_speed_1;
			if (cIsReturning) {
				toRoute = toRoute + returnGroupPaths[cGroup];
			} else {
				toRoute = toRoute + groupPaths[pGroup];
			}
		}
	}
	// my life got 12 hours, 38 minutes and 44 seconds shorter because of this single function
}

void Pathfinder::traceGroup(const OreGroup& group, const coord_t entry, const coord_t exit, route_t& toRoute) {
	toRoute.instructions.resize(0);
	toRoute.floatingInstruction = 0;
	toRoute.push_back(instruction_t::set_speed_1);

	if (entry != exit) {
		toRoute.push_back(instruction_t::mine);
	}

	// exclude entry and exit
	std::vector<coord_t> unmined = group.tiles;
	for (auto i = static_cast<int16_t>(unmined.size() - 1); i >= 0; i--) {
		if (unmined[i] == exit || unmined[i] == entry) {
			std::swap(unmined[i], unmined.back());
			unmined.pop_back();
		}
	}
	coord_t currentPos = entry;

	// Warnsdorff's logic
	while (!unmined.empty()) {
		// get where to go next
		uint8_t nextHop = -1;
		uint8_t minDistance = -1;
		uint8_t minNeighbours = -1;
		for (uint8_t i = 0; i < static_cast<uint8_t>(unmined.size()); i++) {

			if (const uint8_t distance = currentPos - unmined[i]; distance == minDistance) {
				// get neighbors
				uint8_t neighbours = 0;
				for (coord_t neighbourTile : unmined) {
					if (unmined[i] - neighbourTile == 1) {
						neighbours++;
					}
				}

				// if a node with fewer neighbors found
				if (neighbours < minNeighbours) {
					minNeighbours = neighbours;
					nextHop = i;
				}

			// if a closer node is found
			} else if (distance < minDistance) {
				minDistance = distance;

				uint8_t neighbours = 0;
				for (coord_t neighbourTile : unmined) {
					if (unmined[i] - neighbourTile == 1) {
						neighbours++;
					}
				}
				minNeighbours = neighbours;
				nextHop = i;
			}
		}

		// if we can go there directly
		if (minDistance == 1) {
			toRoute.push_back(currentPos.getInstructionTo(unmined[nextHop]));
			toRoute.push_back(instruction_t::mine);

		// get the path there with A*
		} else {
			auto path = Path(currentPos, unmined[nextHop]);
			for (uint16_t i = 1; i < static_cast<uint16_t>(path.path.size()); i++) {
				toRoute.push_back(path.path[i - 1].getInstructionTo(path.path[i]));
			}
			toRoute.push_back(instruction_t::mine);
		}
		currentPos = unmined[nextHop];
		std::swap(unmined[nextHop], unmined.back());
		unmined.pop_back();
	}

	// go to end pos directly
	if (const uint8_t endDistance = currentPos - exit; endDistance == 1) {
		toRoute.push_back(currentPos.getInstructionTo(exit));

	// go to and with A*
	} else {
		const auto path = Path(currentPos, exit);
		for (uint16_t i = 1; i < static_cast<uint16_t>(path.path.size()); i++) {
			toRoute.push_back(path.path[i - 1].getInstructionTo(path.path[i]));
		}
	}
	toRoute.push_back(instruction_t::mine);
}