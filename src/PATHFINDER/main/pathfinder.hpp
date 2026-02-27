#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP

#include <vector>
#include <cstdint>
#include <array>
#include <string>

/**
 * Pathfinder calculate a route threw the map with a genetic algorithm to maximalize collected value on the run.
 */
class Pathfinder {
	public:
		/**
		 * Holy constructor sigma 67 gg
		 * @param timeLimit Time limit in the simulation for the rover to complete the task
		 * @param mapPath Absolute path to the file containing the map
		 */
		Pathfinder(uint16_t timeLimit, const std::string& mapPath);

		/**
		 * Map tile names
		 */
		enum class tile_t : uint8_t {
			start = 0,
			land,
			wall,
			blue = 128,
			yellow = 129,
			green = 130,
			blue_grouped = 192,
			yellow_grouped = 193,
			green_grouped = 194
		};

		/**
		 * coordinates type {x, y}
		 */
		using coord_t = std::array<uint8_t, 2>;

	private:
		// constants
		static constexpr uint8_t   MAP_WIDTH = 50; // width and height of the map is equal
		static constexpr uint8_t GROUP_LIMIT = 9;  // the max number of ores stored in a group, 0 = no limit

		// classes
		class OreGroup {
			public:
				const tile_t oreType;
				const uint16_t oreValue;
				std::array<coord_t, GROUP_LIMIT> tiles{};

				OreGroup(const tile_t oreType, const uint16_t oreValue) : oreType(oreType), oreValue(oreValue) {}
				void collect(coord_t start, std::vector<OreGroup>& groupContainer);
		};

		// variables
		coord_t startPos{};
		const uint16_t timeLimit;
		std::array<tile_t, MAP_WIDTH*MAP_WIDTH> map{};
		std::vector<OreGroup> oreGroups;

		// functions
		[[nodiscard]] static int getIndex(const int x, const int y) {return y * MAP_WIDTH + x;}
		[[nodiscard]] static int getIndex(const coord_t& coords) {return coords[0] * MAP_WIDTH + coords[1];}

		void groupOres();
};

#endif // VD26_PATHFINDER_HPP
