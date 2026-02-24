#ifndef VD26_PATHFINDER_HPP
#define VD26_PATHFINDER_HPP

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
    static constexpr uint16_t MAP_WIDTH = 50;
    const std::array<tile_t, MAP_WIDTH*MAP_WIDTH> map;

    public:

};


#endif // VD26_PATHFINDER_HPP