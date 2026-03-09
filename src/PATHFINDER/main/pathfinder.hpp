#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP

#include <cassert>
#include <vector>
#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <random>

/**
 * Pathfinder calculates a route through a map with a genetic algorithm to maximalize collected value on the run.
 */
class Pathfinder {
	public:
		// constants
		static constexpr uint8_t MAP_WIDTH = 50;
		static constexpr uint8_t GROUP_LIMIT = 9;
		static constexpr uint8_t START_TIME = 0; // how many half hours after 0:00

		// genetic algorithm settings
		static constexpr uint16_t GENETIC_ITERS   = 600; // number of generations
		static constexpr uint16_t GENERATION_SIZE = 400; // number of genomes in a generation

		static constexpr uint16_t ELITISM         = GENERATION_SIZE *  5 / 100; // number of elements to be passed along by elitism

		static constexpr uint16_t BREEDING                   = GENERATION_SIZE * 65 / 100;   // number of elements to be passed along by breeding
		static constexpr uint16_t BREEDING_TOURNAMENT_SIZE   = 5;                            // number of gladiators in a tournament
		static constexpr uint16_t BREEDING_MUTATION          = BREEDING * 15 / 100;          // number of elements to be mutated from breeds
		static constexpr uint16_t BREEDING_SWAP              = BREEDING_MUTATION * 20 / 100; // distribution per mutation type
		static constexpr uint16_t BREEDING_SCRAMBLE          = BREEDING_MUTATION * 15 / 100;
		static constexpr uint16_t BREEDING_INSERTION         = BREEDING_MUTATION * 15 / 100;
		static constexpr uint16_t BREEDING_INVERSION         = BREEDING_MUTATION - BREEDING_SWAP - BREEDING_SCRAMBLE - BREEDING_INSERTION;

		static constexpr uint16_t CLONING                 = GENERATION_SIZE * 20 / 100;    // number of elements to be passed along by cloning
		static constexpr uint16_t CLONING_TOURNAMENT_SIZE = 5;                             // -||-
		static constexpr uint16_t CLONING_SWAP            = CLONING * 20 / 100;
		static constexpr uint16_t CLONING_SCRAMBLE        = CLONING * 15 / 100;
		static constexpr uint16_t CLONING_INSERTION       = CLONING * 15 / 100;
		static constexpr uint16_t CLONING_INVERSION       = CLONING - CLONING_SWAP - CLONING_SCRAMBLE - CLONING_INSERTION;

		static constexpr uint16_t RANDOM_NEW = GENERATION_SIZE - ELITISM - BREEDING - CLONING; // number of elements to be randomly generated to fill the remaining spots in a generation

		static constexpr uint8_t  INDEX_COLLISION_RETRIES = 5; // amount of retries to select a unique member from the generation compared to a pair

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

		/**
		 * Calculates an optimal path for the rover
		 */
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

		/**
		 * Direction type
		 */
		struct direction_t{
			int8_t x;
			int8_t y;

			[[nodiscard]] direction_t operator-() const {
				return { static_cast<int8_t>(-x), static_cast<int8_t>(-y) };
			}

			[[nodiscard]] bool operator==(const direction_t other) const {
				return x == other.x && y == other.y;
			}

			[[nodiscard]] bool operator!=(const direction_t other) const {
				return x != other.x || y != other.y;
			}
		};

		/**
		 * Direction names
		 */
		struct Directions {
			static constexpr direction_t UP_LEFT     {-1, -1};
			static constexpr direction_t UP          { 0, -1};
			static constexpr direction_t UP_RIGHT    { 1, -1};
			static constexpr direction_t RIGHT       { 1,  0};
			static constexpr direction_t DOWN_RIGHT  { 1,  1};
			static constexpr direction_t DOWN        { 0,  1};
			static constexpr direction_t DOWN_LEFT   {-1,  1};
			static constexpr direction_t LEFT        {-1,  0};
			static constexpr direction_t NODIRECTION { 0,  0};

			static constexpr std::array<direction_t, 8> ALL = {
				UP_LEFT, UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT
			};
		};

		/**
		 * Map type
		 */
		using map_t = std::array<tile_t, MAP_WIDTH * MAP_WIDTH>;

		/**
		 * Coordinates type {x, y}
		 */
		struct coord_t {
			uint8_t x;
			uint8_t y;

			[[nodiscard]] bool operator==(const coord_t& other) const {
				return x == other.x && y == other.y;
			}

			[[nodiscard]] bool operator!=(const coord_t& other) const {
				return x != other.x || y != other.y;
			}

			[[nodiscard]] coord_t operator+(const direction_t & direction) const {
				return {static_cast<uint8_t>(x + direction.x), static_cast<uint8_t>(y + direction.y)};
			}

			/**
			 * Checks if the direction points out of the map
			 * @param direction The direction to compare to
			 * @return If the coordinate will be in the map if the direction is added
			 */
			[[nodiscard]] bool operator<(const direction_t & direction) const {
				const int newX = x + direction.x;
				const int newY = y + direction.y;
				if (newX < 0 || newX >= MAP_WIDTH || newY < 0 || newY >= MAP_WIDTH) {
					return true;
				}
				return false;
			}
		};

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
				std::vector<coord_t> path; // stays empty after constructor if no path found

				explicit Path(size_t a, size_t b);
			private:
				[[nodiscard]] static uint8_t getChebyshev(const coord_t coordA, const coord_t coordB) {
					return max(std::abs(coordA.x - coordB.x), std::abs(coordA.y - coordB.y));
				}
				[[nodiscard]] static uint16_t getSquaredDiagonal(const coord_t coordA, const coord_t coordB) {
					const uint8_t x = std::abs(coordA.x - coordB.x);
					const uint8_t y = std::abs(coordA.y - coordB.y);
					return x * x + y * y;
				}
				void getClosestTiles();
				void aStar();

				struct Node {
					coord_t coords{};
					uint16_t g; // Cost from start
					uint16_t f; // Estimated whole cost
					direction_t parent{}; // direction to parent node

					Node(const coord_t coords, const uint16_t g, const coord_t endPos,
					const direction_t parent = Directions::NODIRECTION) {
						this->coords = coords;
						this->g = g;
						this->parent = parent;
						f = getChebyshev(coords, endPos) + g;
					}

					[[nodiscard]] bool operator>(const Node& n) const {
						return f > n.f;
					}
				};

				struct CoordHash {
					std::size_t operator()(const coord_t& c) const {
						return static_cast<std::size_t>(c.x<<8 | c.y);
					}
				};
		};

		struct Genome {
			std::vector<uint16_t> dna;
			int32_t score = 0;
			inline static std::uniform_int_distribution<uint16_t> index_dist;

			static void initDistribution(const uint16_t size) {
				index_dist = std::uniform_int_distribution<uint16_t>(0, size - 1);
			}

			void getFitness() {
				assert(!dna.empty());
				score = fitness(this);
			}

			bool operator>(const Genome& other) const {
				return score > other.score;
			}

			Genome operator+(const Genome& other) const; // breeding

			void swap();
			void scramble();
			void insertion();
			void inversion();
		};

		// variables
		static inline Pathfinder* pathfinder = nullptr;
		inline static std::mt19937 gen{std::random_device{}()};
		inline static auto generation_dist = std::uniform_int_distribution<uint16_t>(0, GENERATION_SIZE - 1);
		coord_t startPos{};
		const uint16_t timeLimit;
		map_t map{};
		std::vector<OreGroup> oreGroups;
		std::vector<Path> paths;

		// functions
		bool calculateBatteryAndTimeUsage(const Path* pathtocheck,uint8_t &startBattery, uint64_t &starttime, const uint8_t speed);
		void GeneticAlgorithm() const;
		void generatePath(std::vector<uint16_t>& path);
		static uint16_t tournamentSelect(const std::vector<Genome>& generation);
		static uint16_t tournamentSelect(const std::vector<Genome>& generation, uint16_t unwantedParticipant);
		static int32_t fitness(const Genome* genome);
		void simulate(std::vector<uint16_t> path,uint64_t *usedTime,uint32_t *gateredOreValue,uint16_t*groupCount);

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
		[[nodiscard]] static uint32_t max(const uint32_t a, const uint32_t b){return a > b ? a : b;}
		[[nodiscard]] static uint32_t min(const uint32_t a, const uint32_t b){return a < b ? a : b;}
		[[nodiscard]] uint32_t getPathIndex(const uint32_t a, const uint32_t b) const {
			assert(a != b);
			assert(oreGroups.size() > 0);
			assert(paths.size() > 0);
			assert(a < oreGroups.size() && b < oreGroups.size());
			const uint32_t g1 = min(a, b);
			const uint32_t g2 = max(a, b);
			const uint32_t n = oreGroups.size() - g1;
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
