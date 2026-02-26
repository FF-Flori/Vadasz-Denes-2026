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
			start,
			land,
			wall,
			blue,
			yellow,
			green
		};

		/**
		 * coordinates type {x, y}
		 */
		using coord_t = std::array<uint8_t, 2>;

	private:
		// constants
		static constexpr uint8_t MAP_WIDTH = 50;
		static constexpr uint8_t GROUP_LIMIT = 9;

		// classes
		class OreGroup {
			public:
				const tile_t ore;
				std::vector<coord_t> tiles;

				OreGroup(tile_t ore);
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
