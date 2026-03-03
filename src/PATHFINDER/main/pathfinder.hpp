#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP

#include <algorithm>
#include <cassert>
#include <vector>
#include <cstdint>
#include <array>
#include <string>

/**
 * Pathfinder calculates a route through a map with a genetic algorithm to maximalize collected value on the run.
 */
class Pathfinder {
	public:
		// constants
		static constexpr uint8_t MAP_WIDTH = 50;
		static constexpr uint8_t GROUP_LIMIT = 9;
		static constexpr int8_t directions[8][2] = {{}};

		/**
		 * This method creates the singleton instance for Pathfinder
		 * @param timeLimit Time limit in the simulation for the rover to complete the task
		 * @param mapPath Absolute path to the file containing the map
		 */
		static void create(const uint16_t timeLimit, const std::string& mapPath) {
			if (pathfinder == nullptr) {
				pathfinder = new Pathfinder(timeLimit, mapPath);
			}
		}

		/**
		 * Deletes the instance
		 */
		static void destroy() {
			if (pathfinder != nullptr) {
				delete pathfinder;
				pathfinder = nullptr;
			}
		}

		/**
		 * Returns the singleton instance of Pathfinder
		 * @return Instance of Pathfinder
		 * @return nullptr
		 */
		static Pathfinder& getInstance() {
			return *pathfinder;
		}

		void calculate();

		/**
		 * Map tile names
		 */
		enum class tile_t : uint8_t {
			wall = 0,
			start,
			land,
			blue,
			yellow,
			green,
			grouped = 255
		};

		typedef struct {
			int8_t x;
			int8_t y;
		} direction_t;

		struct Directions {
			static constexpr direction_t UP_LEFT    {-1, -1};
			static constexpr direction_t UP         {0 , -1};
			static constexpr direction_t UP_RIGHT   {1 , -1};
			static constexpr direction_t RIGHT      {1 ,  0};
			static constexpr direction_t DOWN_RIGHT {1 ,  1};
			static constexpr direction_t DOWN       {0 ,  1};
			static constexpr direction_t DOWN_LEFT  {-1,  1};
			static constexpr direction_t LEFT       {-1,  0};

			static constexpr std::array<direction_t, 8> ALL = {
				UP_LEFT, UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT
			};
		};

		/**
		 * map type
		 */
		using map_t = std::array<tile_t, MAP_WIDTH * MAP_WIDTH>;

		/**
		 * coordinates type {x, y}
		 */
		typedef struct{
			uint8_t x;
			uint8_t y;
		}coord_t;

		// ban copying
		Pathfinder(const Pathfinder&) = delete;
		Pathfinder& operator=(const Pathfinder&) = delete;
		Pathfinder(Pathfinder&&) = delete;
		Pathfinder& operator=(Pathfinder&&) = delete;

	private:
		/**
		 * Holy constructor sigma 67 gg
		 * @param timeLimit Time limit in the simulation for the rover to complete the task
		 * @param mapPath Absolute path to the file containing the map
		 */
		Pathfinder(uint16_t timeLimit, const std::string& mapPath);

		// classes
		class OreGroup {
			public:
				const tile_t ore; // ore type
				const uint8_t oreValue; // value per ore
				std::vector<coord_t> tiles;

				explicit OreGroup(tile_t ore, uint8_t oreValue = 1);
		};

		class Path {
			public:
				const size_t groupA; // group A index
				const size_t groupB; // group B index
				coord_t startPos{};
				coord_t endPos{};
				std::vector<coord_t> path;

				explicit Path(size_t a, size_t b);
			private:
				[[nodiscard]] static uint8_t getChebyshev(const coord_t coordA, const coord_t coordB) {
					return max(std::abs(coordA.x - coordB.x), std::abs(coordA.y - coordB.y));
				}
				void getClosestTiles();
				void aStar();
		};

		// variables
		static inline Pathfinder* pathfinder = nullptr;
		coord_t startPos{};
		const uint16_t timeLimit;
		map_t map{};
		std::vector<OreGroup> oreGroups;
		std::vector<Path> paths;

		// functions
		[[nodiscard]] static int getIndex(const int x, const int y) {return y * MAP_WIDTH + x;}
		[[nodiscard]] static int getIndex(const coord_t coords) {return coords.x + MAP_WIDTH * coords.y;}

		/** Explanation:
		 *    [0][1][2][3]
		 * [0]    .  .  .
		 * [1]       .  .
		 * [2]          .
		 * [3]
		 * The path1 and path2 arguments would be the indeces of this 2D lookup table, but half of the table is not in the flattened array, so we have to ensure that those wont be accessed that is why we create the x and y variables.
		 * To get the index in the 1D array we need two things:
		 *  - The index where the nth line starts in the 1D array
		 *  - The offset from that line to the desired element (basically the x coordinate in the 2D array
		 * We will have to add together these two values to get the index in the 1D array
		 * The offset:
		 * We can get the offset pretty easily. It is just x-y-1 because:
		 *    [0][1][2]
		 * [0]    .  .
		 * [1]       .
		 * [2]
		 * In each row, the elements start at column y+1, so to get the offset we just need to subtract y+1 from x
		 * x-(y+1) => x-y-1
		 * This just ensures that x points to the nht column relative to the starting positions of the elements in that row
		 *
		 * The index of the row in the 1D array:
		 * We know that from the back to the front, the amount of elements in a row will just be the natural numbers and 0: 0,1,2,3,4,5
		 * (The nth row (from the back) will have n elements)
		 * So the offset from the back to that row's first element is just the sum of all integers up to that point (inclusive)
		 * Which we can get with this formula:
		 * endpt*(endpt+1)/2
		 * and to make it count from the start not the end, we just need to subtract it from the number of elemenrs in the list, which is already stored in  the pathsSize variable
		 * (I added the bitshift for speed)
		 *
		 * https://www.desmos.com/calculator/ktpkbeylkv
		 */
		[[nodiscard]] static unsigned int max(const unsigned int a, const unsigned int b){return a > b ? a : b;}
		[[nodiscard]] static unsigned int min(const unsigned int a, const unsigned int b){return a < b ? a : b;}
		[[nodiscard]] size_t getPathIndex(const size_t a, const size_t b) const {
			assert(a != b);
			assert(oreGroups.size() > 0);
			assert(paths.size() > 0);
			assert(a < oreGroups.size() && b < oreGroups.size());
			const size_t g1 = min(a, b);
			const size_t g2 = max(a, b);
			const size_t n = oreGroups.size() - g1;
			return paths.size() - ((n*(n-1)) >> 1) + g2 - g1 - 1;
		}

		void groupOres();
		void createGroup(uint8_t x, uint8_t y);
		/**
		 * This function checks if the point at the supplied coordinates is the specified ore or not, and if it is, it adds it to the group and changes the value on the map to grouped
		 * @param x The x coordinate of the checked point
		 * @param y The y coordinate of the checked point
		 * @param oreType The type of ore the checked point needs to be
		 * @param group The group to append the ore to
		 */
		void checkCoord(uint8_t x, uint8_t y, tile_t oreType, OreGroup& group);
};

#endif // VD26_PATHFINDER_HPP
