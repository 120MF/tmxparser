#pragma once

#include <tl/expected.hpp>
#include <string>
#include <filesystem>
#include <pugixml.hpp>
#include "../Map.hpp"

namespace tmx {

class Parser {
public:
    using Result = tl::expected<Map, std::string>;
    
    static auto parseFromFile(const std::filesystem::path& path) -> Result;
    static auto parseFromString(const std::string& xml) -> Result;
    
private:
    static auto parseMap(const pugi::xml_node& mapNode) -> Result;
    static auto parseTileset(const pugi::xml_node& tilesetNode) -> tl::expected<Tileset, std::string>;
    static auto parseLayer(const pugi::xml_node& layerNode) -> tl::expected<Layer, std::string>;
    static auto parseProperties(const pugi::xml_node& propertiesNode) -> Properties;
    static auto parseOrientation(const std::string& str) -> Orientation;
    static auto parseRenderOrder(const std::string& str) -> RenderOrder;
    static auto parseData(const pugi::xml_node& dataNode, std::uint32_t width, std::uint32_t height) 
        -> tl::expected<std::vector<std::uint32_t>, std::string>;
};

}