#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP
#include <vector>

#include <cstdint>
#include <array>
#include <string>



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
		int startPos[2];

		const uint16_t timeLimit;
		static constexpr uint16_t MAP_WIDTH = 50;
		std::array<tile_t, MAP_WIDTH*MAP_WIDTH> map;
		[[nodiscard]] static int getIndex(const int x, const int y) {return y * MAP_WIDTH + x;}

	public:
		/**
		 * Holy constructor sigma 67 gg
		 * @param timeLimit Time limit in the simulation for the rover to complete the task
		 * @param mapPath Absolute path to the file containing the map
		 */
		Pathfinder(uint16_t timeLimit, const std::string& mapPath);
};

#endif // VD26_PATHFINDER_HPP
