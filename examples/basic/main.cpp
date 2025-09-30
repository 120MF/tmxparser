#include <cstring>
#include <iostream>
#include <tmx/tmx.hpp>

int main()
{
    std::cout << "TMX Parser Basic Example" << std::endl;

    // Parse a TMX file
    auto result = tmx::Parser::parseFromFile(std::string(ASSET_DIR) + "test.tmx");

    if (!result)
    {
        std::cerr << "Failed to parse TMX file: " << result.error() << std::endl;
        return 1;
    }

    const auto& map = *result;

    std::cout << "Successfully parsed TMX map:" << std::endl;
    std::cout << "  Version: " << map.version << std::endl;
    std::cout << "  Size: " << map.width << "x" << map.height << std::endl;
    std::cout << "  Tile size: " << map.tilewidth << "x" << map.tileheight << std::endl;
    std::cout << "  Tilesets: " << map.tilesets.size() << std::endl;
    std::cout << "  Layers: " << map.layers.size() << std::endl;

    for (const auto& tileset : map.tilesets)
    {
        std::cout << "    Tileset: " << tileset.name
            << " (firstgid=" << tileset.firstgid
            << ", tiles=" << tileset.tilecount << ")" << std::endl;
    }

    for (const auto& layer : map.layers)
    {
        std::cout << "    Layer: " << layer.name
            << " (" << layer.width << "x" << layer.height
            << ", tiles=" << layer.data.size() << ")" << std::endl;
    }

    std::cout << "\nNote: TMXParser is a parsing library. Rendering should be implemented in your application using SDL3, SFML, or other graphics libraries." << std::endl;

    return 0;
}
