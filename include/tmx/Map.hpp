#pragma once

#include <tl/expected.hpp>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace tmx::map {

enum class Orientation {
    Orthogonal,
    Isometric,
    Staggered,
    Hexagonal
};

enum class RenderOrder {
    RightDown,
    RightUp,
    LeftDown,
    LeftUp
};

struct Color {
    std::uint8_t r, g, b, a = 255;
    
    Color() = default;
    Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
    
    static auto fromString(const std::string& hex) -> tl::expected<Color, std::string>;
};

struct Property {
    std::string name;
    std::string value;
    std::string type;
};

struct Properties {
    std::vector<Property> properties;
    
    auto get(const std::string& name) const -> std::string;
    auto getInt(const std::string& name, int defaultValue = 0) const -> int;
    auto getFloat(const std::string& name, float defaultValue = 0.0f) const -> float;
    auto getBool(const std::string& name, bool defaultValue = false) const -> bool;
};

struct Tileset {
    std::uint32_t firstgid;
    std::string name;
    std::uint32_t tilewidth;
    std::uint32_t tileheight;
    std::uint32_t tilecount;
    std::uint32_t columns;
    std::string source;  // For external tilesets
    std::string image;
    std::uint32_t imagewidth;
    std::uint32_t imageheight;
    Properties properties;
};

struct Chunk {
    std::int32_t x, y;
    std::uint32_t width, height;
    std::vector<std::uint32_t> data;
};

struct Layer {
    std::string name;
    std::uint32_t width, height;
    std::vector<std::uint32_t> data;
    std::vector<Chunk> chunks;  // For infinite maps
    bool visible = true;
    float opacity = 1.0f;
    Properties properties;
};

struct ObjectGroup {
    std::string name;
    bool visible = true;
    float opacity = 1.0f;
    Properties properties;
    // TODO: Add objects
};

struct Map {
    std::string version = "1.0";
    std::string tiledversion;
    Orientation orientation = Orientation::Orthogonal;
    RenderOrder renderorder = RenderOrder::RightDown;
    std::uint32_t width, height;
    std::uint32_t tilewidth, tileheight;
    bool infinite = false;
    Color backgroundcolor;
    std::uint32_t nextlayerid = 1;
    std::uint32_t nextobjectid = 1;
    
    std::vector<Tileset> tilesets;
    std::vector<Layer> layers;
    std::vector<ObjectGroup> objectgroups;
    Properties properties;
};

}