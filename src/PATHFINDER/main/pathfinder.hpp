#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP
#include <vector>

#include <cstdint>
#include <array>

enum class tile_t : uint8_t {
    start,
    land,
    wall,
    blue,
    yellow,
    green
};

class Pathfinder {
	private:
		int startPos[2] = {0,0};
		char* table;
		int width;
		int height;
		static constexpr uint16_t MAP_WIDTH = 50;
		const std::array<tile_t, MAP_WIDTH*MAP_WIDTH> map;
	public:
		Pathfinder();
		// Mivel gondolom hogy sorok listája lesz majd az input
		void inputTransformation(std::vector<std::vector<char>> inputList);

    public:

};


#endif // VD26_PATHFINDER_HPP
