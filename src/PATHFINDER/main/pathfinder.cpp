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
	}
	std::cout << "Genomes generated\n";

	// sort the elements for the next generation
	std::partial_sort(generation.begin(), generation.begin() + ELITISM, generation.end(), std::greater<>());

	std::cout << "Starting generations\n";
	try {
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
	} catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}

	std::cout << "Genetic algorythm finished!" << std::endl << "Best genome's groups:" << std::endl;
	for (const uint16_t gr : generation[0].dna) {
		std::cout << gr << " (" << oreGroups[gr].tiles[0].x << "; " << oreGroups[gr].tiles[0].y << ")" << std::endl;
	}
	// TODO: simulate the best genome and save it (and remove test couts above and everywhere else)
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

uint32_t Pathfinder::fitness(const Genome* genome) const {
	// helper tables in local thread
	thread_local std::vector<uint8_t> memoTable;    // best battery for a state
	thread_local std::vector<uint32_t> versionTable; // version of the state stored in memoTable (helps to skip clearing memoTable)
	thread_local uint32_t currentVersion = 0;

	// count of every possible bfsState (without isReturning)
	static const size_t stateSize = oreGroups.size() * maxDistPerSegment * (timeLimit + 1);

	// new version to skip wiping off memoTable
	currentVersion++;

	uint16_t returnedOres = 0; // value of returned ores
	uint16_t lastReachableGroupIndex = oreGroups.size() - 1; // last reachable group (decreases if a dead segment is found)

	// fill up memoTable with starter values if empty
	if (const size_t requiredSize = stateSize * 2;
	memoTable.size() < requiredSize) {
		memoTable.resize(requiredSize, 0);
		versionTable.resize(requiredSize, 0);
	}

	// get group-group and group-start distances
	std::vector<uint16_t> distances;
	distances.reserve(oreGroups.size());
	distances.push_back(paths[getPathIndex(oreGroups.size() - 1, genome->dna[0])].path.size() - 1);
	std::vector<uint16_t> returnDistances;
	returnDistances.reserve(oreGroups.size());
	returnDistances.push_back(distances[0]);
	for (size_t g = 1; g < oreGroups.size(); g++) {
		distances.push_back(paths[getPathIndex(genome->dna[g - 1], genome->dna[g])].path.size() - 1);
		returnDistances.push_back(paths[getPathIndex(oreGroups.size() - 1, genome->dna[g])].path.size() - 1);
	}

	// bfs queue with start pos
	std::queue<bfsState> q;
	q.push({0, 0, 0, START_BATTERY});

	while(!q.empty()) {
		auto [groupIndex, dist, time, battery, isReturning] = q.front();
		q.pop();

		if (isReturning && groupIndex + 1 <= returnedOres) {
			continue;
		}

		if (groupIndex > lastReachableGroupIndex) {
			continue;
		}

		// get current path segment's length
		const uint16_t currentSegmentLength = isReturning ? returnDistances[groupIndex] : distances[groupIndex];

		// if the segment is finished
		if (dist >= currentSegmentLength) {
			// reached start
			if (isReturning) {
				returnedOres = groupIndex + 1;

				// if all groups collected
				if (returnedOres == oreGroups.size()) [[unlikely]] {break;}

				// reached an ore group - simulate the rover inside the group
			} else {
				// slightly overthrow group action count to have correct estimation on unusual groups
				const uint16_t targetActions = oreGroups[groupIndex].tiles.size() * 11 / 5; // *11/5 = *2.2
				uint8_t cycleTime = time % 48;
				uint16_t elapsedTime = 0;
				for (uint16_t t = 0; t < targetActions;) {
					elapsedTime++;
					cycleTime++;
					if (cycleTime >= 48) {
						cycleTime = 0;
					}
					if (cycleTime <= 32) {
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
						return 0;
					}
					// the last reachable group is the previous one
					lastReachableGroupIndex = groupIndex - 1;
					if (lastReachableGroupIndex + 1 <= returnedOres) [[unlikely]] {break;} // if the max possible ore groups are reached
					continue;
				}

				time += elapsedTime;

				if (groupIndex + 1 > returnedOres) {
					q.push({groupIndex, 0, time, battery, true});
				}
				if (groupIndex < lastReachableGroupIndex) {
					q.push({static_cast<uint16_t>(groupIndex + 1), 0, time, battery});
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
				return 0;
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
				const size_t returningOffset = isReturning ? stateSize : 0;
				const size_t index = returningOffset + (groupIndex * maxDistPerSegment + nextDist) * (timeLimit + 1) + nextTime;

				// new or better state than saved
				if (versionTable[index] != currentVersion || memoTable[index] < nextBattery) {
					versionTable[index] = currentVersion;
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
				const size_t returningOffset = isReturning ? stateSize : 0;
				const size_t index = returningOffset + (groupIndex * maxDistPerSegment + nextDist) * (timeLimit + 1) + nextTime;

				// if found new or better state than saved
				if (versionTable[index] != currentVersion || memoTable[index] < nextBattery) {
					versionTable[index] = currentVersion;
					memoTable[index] = nextBattery;
					q.push({groupIndex, nextDist, nextTime, nextBattery, isReturning});
				}
			}
		}
	}

	// calculate collected ores' sum
	uint32_t returnedValue = 0;
	for (uint16_t i = 0; i < returnedOres; i++) {
		returnedValue += oreGroups[genome->dna[i]].tiles.size() * oreGroups[genome->dna[i]].oreValue;
	}

	// my life got 12 hours, 38 minutes and 44 seconds shorter because of this single function
	return returnedValue;
}