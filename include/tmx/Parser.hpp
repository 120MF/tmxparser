#pragma once

#include <tl/expected.hpp>
#include <string>
#include <filesystem>
#include <pugixml.hpp>
#include "Map.hpp"

namespace tmx {

class Parser {
public:
    static auto parseFromFile(const std::filesystem::path& path) -> tl::expected<map::Map, std::string>;
    static auto parseFromString(const std::string& xml) -> tl::expected<map::Map, std::string>;
    
private:
    static auto parseMap(const pugi::xml_node& mapNode, const std::filesystem::path& basePath = "") -> tl::expected<map::Map, std::string>;
    static auto parseTileset(const pugi::xml_node& tilesetNode, const std::filesystem::path& basePath = "") -> tl::expected<map::Tileset, std::string>;
    static auto parseTilesetFile(const std::filesystem::path& path, std::uint32_t firstgid) -> tl::expected<map::Tileset, std::string>;
    static auto parseTile(const pugi::xml_node& tileNode) -> tl::expected<map::Tile, std::string>;
    static auto parseAnimation(const pugi::xml_node& animationNode) -> tl::expected<map::Animation, std::string>;
    static auto parseLayer(const pugi::xml_node& layerNode) -> tl::expected<map::Layer, std::string>;
    static auto parseProperties(const pugi::xml_node& propertiesNode) -> map::Properties;
    static auto parseOrientation(const std::string& str) -> map::Orientation;
    static auto parseRenderOrder(const std::string& str) -> map::RenderOrder;
    static auto parseData(const pugi::xml_node& dataNode, std::uint32_t width, std::uint32_t height) 
        -> tl::expected<std::vector<std::uint32_t>, std::string>;
};

}
