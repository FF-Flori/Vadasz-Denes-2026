#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP
#include <vector>


class Pathfinder {
	private:
		int startPos[2] = {0,0};
		char* table;
	public:
		Pathfinder();
		// Mivel gondolom hogy sorok listája lesz majd az input
		void inputTransformation(std::vector<std::vector<char>> inputList);
};


#endif // VD26_PATHFINDER_HPP
