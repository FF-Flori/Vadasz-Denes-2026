#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP

#include <algorithm>
#include <cassert>
#include <vector>
#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <random>
#include <stdexcept>

/**
 * Pathfinder calculates a route through a map with a genetic algorithm to maximalize collected value on the run.
 * @authors FF-Flori, HiAndris
 */
class Pathfinder {
	public:
		// constants
		// global settings
		static constexpr uint8_t MAP_WIDTH = 50;
		static constexpr uint8_t GROUP_LIMIT = 9;
		static constexpr uint8_t START_TIME = 0; // how many half hours after 0:00
		static constexpr uint8_t START_BATTERY = 100; // battery percentage at start

		// genetic algorithm settings
		static constexpr uint16_t GENETIC_ITERS   = 1600; // number of generations
		static constexpr uint16_t GENERATION_SIZE = 900; // number of genomes in a generation

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

		// simulation settings
		static constexpr float TIME_PER_TILE_DAY   = 0.451f; // estimated time to pass for a step during the day
		static constexpr float TIME_PER_TILE_NIGHT = 0.727f; // estimated time to pass for a step at night
		static constexpr int8_t STEP_COSTS[2][4] = { // cost of steps per one time unit, the first is 0 steps, should be in ascending order
			{-9, -8, -2,  8}, // daytime
			{ 1,  2,  8, 18} // nighttime
		};

		/**
		 * Creates the singleton instance for Pathfinder
		 * @param timeLimit Time limit in the simulation (in half hours!) for the rover to complete the task
		 * @param mapPath Absolute path to the file containing the map
		 * @return void
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
		 * Direction type vector{x, y}
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
			static constexpr direction_t UP_LEFT      {-1, -1};
			static constexpr direction_t UP           { 0, -1};
			static constexpr direction_t UP_RIGHT     { 1, -1};
			static constexpr direction_t RIGHT        { 1,  0};
			static constexpr direction_t DOWN_RIGHT   { 1,  1};
			static constexpr direction_t DOWN         { 0,  1};
			static constexpr direction_t DOWN_LEFT    {-1,  1};
			static constexpr direction_t LEFT         {-1,  0};
			static constexpr direction_t NO_DIRECTION { 0,  0};

			static constexpr std::array<direction_t, 8> ALL = {
				UP_LEFT, UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT
			};
		};

		/**
		 * Map type
		 */
		using map_t = std::array<tile_t, MAP_WIDTH * MAP_WIDTH>;

		/**
		 * instructions type (half bytes)
		 */
		enum class instruction_t : uint8_t {
			up_left         = 0b0000,
			up              = 0b0001,
			up_right        = 0b0010,
			right           = 0b0011,
			down_right      = 0b0100,
			down            = 0b0101,
			down_left       = 0b0110,
			left            = 0b0111,
			set_speed_0     = 0b1000,
			set_speed_1     = 0b1001,
			set_speed_2     = 0b1010,
			set_speed_3     = 0b1011,
			mine            = 0b1100,
			no_instruction  = 0b1111  // padding instruction, does not take time or resources in the simulation
		};

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

			[[nodiscard]] coord_t operator+(const direction_t& direction) const {
				return {static_cast<uint8_t>(x + direction.x), static_cast<uint8_t>(y + direction.y)};
			}

			/**
			 * Returns the Chebyshev distance between coordinates
			 * @param other Other coordinates
			 * @return Chebyshev distance
			 */
			[[nodiscard]] constexpr uint8_t operator-(const coord_t& other) const {
				return std::max(std::abs(x - other.x), std::abs(y - other.y));
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

			[[nodiscard]] instruction_t getInstructionTo(const coord_t other) const {
				if (other.x == x - 1 && other.y == y - 1) {return instruction_t::up_left;}
				if (other.x == x     && other.y == y - 1) {return instruction_t::up;}
				if (other.x == x + 1 && other.y == y - 1) {return instruction_t::up_right;}
				if (other.x == x + 1 && other.y == y    ) {return instruction_t::right;}
				if (other.x == x + 1 && other.y == y + 1) {return instruction_t::down_right;}
				if (other.x == x     && other.y == y + 1) {return instruction_t::down;}
				if (other.x == x - 1 && other.y == y + 1) {return instruction_t::down_left;}
				if (other.x == x - 1 && other.y == y    ) {return instruction_t::left;}
				return instruction_t::no_instruction;
			}
		};

		/**
		 * instruction container
		 */
		struct route_t {
			std::vector<uint8_t> instructions{};
			uint8_t floatingInstruction = 0;

			void push_back(const instruction_t instruction) {
				if (floatingInstruction & 1) {
					instructions.push_back((floatingInstruction & 0xf0) | (static_cast<uint8_t>(instruction) & 0x0f));
					floatingInstruction--;
				} else {
					floatingInstruction = (static_cast<uint8_t>(instruction) << 4) + 1;
				}
			}

			[[nodiscard]] size_t size() const {
				return instructions.size() * 2 + (floatingInstruction & 1);
			}

			[[nodiscard]] instruction_t operator[](const size_t i) const {
				if (i & 1) {
					return static_cast<instruction_t>(instructions[i / 2] & 0x0f);
				}

				if (i < size() - 1) {
					return static_cast<instruction_t>(instructions[i / 2] >> 4);
				}

				return static_cast<instruction_t>(floatingInstruction >> 4);
			}

			[[nodiscard]] instruction_t at(const size_t i) const {
				if (i >= size()) {
					throw std::out_of_range("pathfinder::at() out of range");
				}

				if (i & 1) {
					return static_cast<instruction_t>(instructions[i / 2] & 0x0f);
				}

				if (i < size() - 1) {
					return static_cast<instruction_t>(instructions[i / 2] >> 4);
				}

				return static_cast<instruction_t>(floatingInstruction >> 4);
			}

			[[nodiscard]] route_t operator+(const route_t& other) const {
				route_t result;
				result.instructions.reserve(size() + other.size() + (floatingInstruction & 1));
				result.instructions = instructions;

				if (floatingInstruction & 1) {
					result.push_back(instruction_t::no_instruction);
				}

				result.instructions.insert(result.instructions.end(), other.instructions.begin(), other.instructions.end());

				result.floatingInstruction = other.floatingInstruction;

				return result;
			}

			void operator+=(const route_t& other) {
				if (floatingInstruction & 1) {
					push_back(instruction_t::no_instruction);
				}

				instructions.insert(instructions.end(), other.instructions.begin(), other.instructions.end());

				floatingInstruction = other.floatingInstruction;
			}

			void invert() {
				// invert means up-left -> down-right, up -> down ...etc.
				for (auto& byte : instructions) {
					if ((byte & 0b10001000) > 0) {
						byte ^= 0b01000100;
					} else {
						if ((byte & 0b00001000) == 0) {
							byte ^= 0b00000100;
						}
						if ((byte & 0b10000000) == 0) {
							byte ^= 0b01000000;
						}
					}
				}
			}

			[[nodiscard]] route_t inverted() const {
				// invert means up-left -> down-right, up -> down ...etc.

				route_t result = *this;

				for (auto& byte : result.instructions) {
					if ((byte & 0b10001000) > 0) {
						byte ^= 0b01000100;
					} else {
						if ((byte & 0b00001000) == 0) {
							byte ^= 0b00000100;
						}
						if ((byte & 0b10000000) == 0) {
							byte ^= 0b01000000;
						}
					}
				}

				return result;
			}

			void reverse() {
				if (floatingInstruction & 1) {
					push_back(instruction_t::no_instruction);
				}
				std::reverse(instructions.begin(), instructions.end());

				for (auto& byte : instructions) {
					byte = ((byte << 4) & 0xf0) | (byte >> 4);
				}
			}

			[[nodiscard]] route_t reversed() const {
				route_t result = *this;

				if (floatingInstruction & 1) {
					result.push_back(instruction_t::no_instruction);
				}
				std::reverse(result.instructions.begin(), result.instructions.end());

				for (auto& byte : result.instructions) {
					byte = ((byte << 4) & 0xf0) | (byte >> 4);
				}

				return result;
			}
		};

		/**
		 * Calculates an optimal path for the rover and returns instructions
		 */
		route_t calculate();

		// no copying tuff
		Pathfinder(const Pathfinder&) = delete;
		Pathfinder& operator=(const Pathfinder&) = delete;
		Pathfinder(Pathfinder&&) = delete;
		Pathfinder& operator=(Pathfinder&&) = delete;

	private:
		/**
		 * Holy constructor sigma 67 gg
		 * @param timeLimit Time limit in the simulation for the rover to complete the task
		 * @param mapPath Absolute path to the file containing the map
		 * @author HiAndris
		 */
		Pathfinder(uint16_t timeLimit, const std::string& mapPath);

		// classes
		class OreGroup {
			public:
				const tile_t ore;      // ore type
				const uint8_t oreValue; // value per ore
				std::vector<coord_t> tiles;

				explicit OreGroup(tile_t ore, uint8_t oreValue = 1);
		};

		class Path {
			public:
				size_t groupA; // group A index
				size_t groupB; // group B index
				coord_t startPos{};
				coord_t endPos{};
				std::vector<coord_t> path; // stays empty after constructor if no path found

				Path(size_t a, size_t b);
				Path(coord_t a, coord_t b);
			private:
				[[nodiscard]] static constexpr uint8_t getChebyshev(const coord_t coordA, const coord_t coordB) {
					return coordA - coordB;
				}
				[[nodiscard]] static constexpr uint16_t getSquaredDiagonal(const coord_t coordA, const coord_t coordB) {
					const uint8_t x = std::abs(coordA.x - coordB.x);
					const uint8_t y = std::abs(coordA.y - coordB.y);
					return x * x + y * y;
				}
				void getClosestTiles();
				void aStar();

				struct Node {
					coord_t coords{};
					uint16_t g;         // Cost from start
					uint16_t f;          // Estimated whole cost
					direction_t parent{}; // direction to parent node

					Node(const coord_t coords, const uint16_t g, const coord_t endPos,
					const direction_t parent = Directions::NO_DIRECTION) {
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
			uint32_t score = 0;
			inline static std::uniform_int_distribution<uint16_t> index_dist;

			static void initDistribution(const uint16_t size) {
				index_dist = std::uniform_int_distribution<uint16_t>(0, size - 1);
			}

			void getFitness() {
				assert(!dna.empty());
				score = pathfinder->fitness(this);
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

		// perfect 8 byte struct no padding gigachad sigma
		struct bfsState {
			uint16_t groupIndex{}; // next group to go to or the group returning from if isReturning is 1 (WARNING: a genome's dna index, not an index from oreGroups)
			uint16_t dist{};        // distance from previous group
			uint16_t time{};         // time passed since start
			uint8_t battery{};        // battery percentage
			uint8_t isReturning = 0;  // 1 if this state is currently returning to the starting position, 0 if not, any other number if this whole state has invalid data

			// difference of states in distance
			uint16_t operator-(const bfsState& other) const {
				// states are incomparable
				if (groupIndex != other.groupIndex || isReturning != other.isReturning) {
					return UINT16_MAX;
				}
				return abs(dist - other.dist);
			}
		};

		// variables
		static inline Pathfinder* pathfinder = nullptr;
		inline static std::mt19937 gen{std::random_device{}()};
		inline static auto generation_dist = std::uniform_int_distribution<uint16_t>(0, GENERATION_SIZE - 1);
		coord_t startPos{};
		const uint16_t timeLimit;
		map_t map{};
		std::vector<OreGroup> oreGroups{};
		std::vector<Path> paths{};
		uint16_t maxDistPerSegment{};

		// functions
		// genetic algorythm functions
		[[nodiscard]] Genome GeneticAlgorithm() const;
		static uint16_t tournamentSelect(const std::vector<Genome>& generation);
		static uint16_t tournamentSelect(const std::vector<Genome>& generation, uint16_t unwantedParticipant);
		uint32_t fitness(const Genome* genome) const;
		void calculateInstructions(const Genome* genome, route_t& toRoute) const;

		// helper functions
		[[nodiscard]] static int getIndex(const int x, const int y) {return y * MAP_WIDTH + x;}
		[[nodiscard]] static int getIndex(const coord_t coords) {return coords.x + MAP_WIDTH * coords.y;}

		/**
		 * Returns the path's index between 2 groups
		 *
		 * @par Explanation
		 * @verbatim
		 *    [0][1][2][3]
		 * [0]    .  .  .
		 * [1]       .  .
		 * [2]          .
		 * [3] @endverbatim
		 * The path1 and path2 arguments would be the x, y of an imaginary 2D lookup table, but half of the table is not in the flattened array.
		 * To get the index in the 1D array, we need two things:
		 *  - The index where the Nth line starts in the 1D array.
		 *  - The offset from that line to the desired element (basically the x coordinate).
		 *
		 * We will have to add these two values together to get the index in the 1D array.
		 * The offset:
		 * We can get the offset pretty easily. It is just x-y-1 because:
		 * @verbatim
		 *    [0][1][2]
		 * [0]    .  .
		 * [1]       .
		 * [2] @endverbatim
		 * In each row, the elements start at column y+1, so to get the offset we just need to subtract y+1 from x:
		 * x-(y+1) => x-y-1
		 * This just ensures that x points to the nht column relative to the starting positions of the elements in that row.
		 * The index of the row in the 1D array:
		 * We know that from the back to the front, the amount of elements in a row will just be the natural numbers and 0.
		 * (The Nth row (from the back) will have n elements.)
		 * So the offset from the back to that row's first element is just the sum of all integers up to that point (inclusive)
		 * Which we can get with this formula:
		 * endpoint * (endpoint + 1) / 2
		 * and to make it count from the start, not the end, we just need to subtract it from the number of elements in the list, which is already stored in  the pathsSize variable.
		 * (I added the bitshift for speed.)
		 *
		 * @par Additional information (from HiAndris)
		 * This project turned out to have so much more complex math equations than this one, but I had no affinity to explain them beautifully like here.
		 * I also made a desmos graph about this problem earlier on, so <a href="https://www.desmos.com/calculator/ktpkbeylkv">here it is</a>.
		 *
		 * @param a Index of a group in the oreGroups vector
		 * @param b Index of a group in the oreGroups vector
		 * @return Index of a path in the paths vector
		 * @authors FF-Flori, HiAndris
		 */
		[[nodiscard]] uint32_t getPathIndex(const uint32_t a, const uint32_t b) const {
			assert(a != b);
			assert(oreGroups.size() > 0);
			assert(paths.size() > 0);
			assert(a < oreGroups.size() && b < oreGroups.size());
			const uint32_t g1 = std::min(a, b);
			const uint32_t g2 = std::max(a, b);
			const uint32_t n = oreGroups.size() - g1;
			return paths.size() - ((n*(n-1)) >> 1) + g2 - g1 - 1;
		}

		// ore group functions
		/**
		 * Groups neighboring ores on the map
		 * @author FF-Flori
		 */
		void groupOres();

		/**
		 * Creates an ore group starting from the x, y coords
		 * @author FF-Flori
		 */
		void createGroup(uint8_t x, uint8_t y);

		/**
		 * This function checks if the point at the supplied coordinates is the specified ore or not, and if it is, it adds it to the group and changes the value on the map to grouped
		 * @param x The x coordinate of the checked point
		 * @param y The y coordinate of the checked point
		 * @param oreType The type of ore the checked point needs to be
		 * @param group The group to append the ore to
		 */
		void checkCoord(int16_t x, int16_t y, tile_t oreType, OreGroup& group);

		/**
		 * Makes a route inside an ore group from a starting position to an end position
		 * @param group oreGroup
		 * @param entry start position
		 * @param exit end position
		 * @param toRoute the container to put the path into
		 */
		static void traceGroup(const OreGroup& group, coord_t entry, coord_t exit, route_t& toRoute);
};

#endif // VD26_PATHFINDER_HPP
