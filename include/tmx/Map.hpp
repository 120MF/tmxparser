#pragma once

#include <tl/expected.hpp>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace tmx::map
{
    enum class Orientation
    {
        Orthogonal,
        Isometric,
        Staggered,
        Hexagonal
    };

    enum class RenderOrder
    {
        RightDown,
        RightUp,
        LeftDown,
        LeftUp
    };

    struct Color
    {
        std::uint8_t r{}, g{}, b{}, a = 255;

        Color() = default;

        Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255)
            : r(r), g(g), b(b), a(a)
        {
        }

        static auto fromString(const std::string& hex) -> tl::expected<Color, std::string>;
    };

    struct Property
    {
        std::string name;
        std::string value;
        std::string type;
    };

    struct Properties
    {
        std::vector<Property> properties;

        [[nodiscard]] auto get(const std::string& name) const -> std::string;
        [[nodiscard]] auto getInt(const std::string& name, int defaultValue = 0) const -> int;
        [[nodiscard]] auto getFloat(const std::string& name, float defaultValue = 0.0f) const -> float;
        [[nodiscard]] auto getBool(const std::string& name, bool defaultValue = false) const -> bool;
    };

    struct Frame
    {
        std::uint32_t tileid;      // The local tile ID within the tileset
        std::uint32_t duration;    // How long (in milliseconds) this frame should be displayed
    };

    struct Animation
    {
        std::vector<Frame> frames;
    };

    struct Tile
    {
        std::uint32_t id;          // Local ID within the tileset
        Properties properties;
        Animation animation;       // Optional animation data
    };

    struct Tileset
    {
        std::uint32_t firstgid;
        std::string name;
        std::uint32_t tilewidth;
        std::uint32_t tileheight;
        std::uint32_t tilecount;
        std::uint32_t columns;
        std::string source; // For external tilesets (.tsx file path)
        std::string image;
        std::uint32_t imagewidth;
        std::uint32_t imageheight;
        Properties properties;
        std::vector<Tile> tiles; // Tiles with animations or properties
    };

    struct Chunk
    {
        std::int32_t x, y;
        std::uint32_t width, height;
        std::vector<std::uint32_t> data;
    };

    struct Layer
    {
        std::string name;
        std::uint32_t width, height;
        std::vector<std::uint32_t> data;
        std::vector<Chunk> chunks; // For infinite maps
        bool visible = true;
        float opacity = 1.0f;
        Properties properties;
    };

    enum class ObjectShape
    {
        Rectangle,  // Default shape (has width and height)
        Ellipse,    // Ellipse shape (has width and height)
        Point,      // Point shape (no width/height)
        Polygon,    // Polygon shape (has points)
        Polyline,   // Polyline shape (has points)
        Text        // Text object
    };

    struct Point
    {
        float x, y;
    };

    struct Object
    {
        std::uint32_t id;
        std::string name;
        std::string type;
        float x, y;           // Position in pixels
        float width, height;  // Size in pixels (for rectangle/ellipse)
        float rotation = 0.0f; // Rotation in degrees
        bool visible = true;
        ObjectShape shape = ObjectShape::Rectangle;
        std::vector<Point> points; // For polygon and polyline
        std::uint32_t gid = 0;     // Global tile ID for tile objects
        Properties properties;
    };

    struct ObjectGroup
    {
        std::string name;
        bool visible = true;
        float opacity = 1.0f;
        Properties properties;
        std::vector<Object> objects;
    };

    struct Map
    {
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
