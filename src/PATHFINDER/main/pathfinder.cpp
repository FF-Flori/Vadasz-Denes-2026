#include "pathfinder.hpp"
#include <cassert>
#include <iostream>


Pathfinder::Pathfinder(){
	std::cout<<"Init\n";
	return;
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

