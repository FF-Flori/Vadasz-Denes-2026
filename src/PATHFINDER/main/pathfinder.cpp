#include "pathfinder.hpp"
#include <cassert>
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
	uint16_t y = 0;

	// read lines
	while (std::getline(file, line) && y < MAP_WIDTH) {
		// read chars
		for (uint16_t x = 0; x < MAP_WIDTH*2 && x < line.length(); x+=2) {
			char c = line[x];

			tile_t type;
			switch (c) {
				case '.': type = tile_t::land; break;
				case '#': type = tile_t::wall; break;
				case 'B': type = tile_t::blue; break;
				case 'G': type = tile_t::green; break;
				case 'Y': type = tile_t::yellow; break;
				case 'S': type = tile_t::start; break;
				default:
					// !!! we can also throw runtime error here
					type = tile_t::land;
					break;
			}

			// append value to map
			map[getIndex(x, y)] = type;
		}
		y++;
	}

	file.close();
}


//Csak akkor fog működni hogy ha mindegyik sor ugyan olyan hosszú
void Pathfinder::inputTransformation(std::vector<std::vector<char>> inputList){
	std::cout<<"Start\n";
	int tablesize = inputList.size()* inputList.at(0).size();
	//tudom hogy a sizeof char az csak simán 1 szóval nem számít, de azért bele szoktam tenni
	table = (char*)malloc(sizeof(char)*(tablesize+1));
	width = inputList.at(0).size();
	height = inputList.size();

	int counter = 0;
	for(std::vector<char> line : inputList){
		for(char currchar : line){
			assert(counter < tablesize);
			table[counter] = currchar;
			counter++;
		}
	}
	table[counter] = '\0';
	std::cout<<table<<"\n";
	return;
}

