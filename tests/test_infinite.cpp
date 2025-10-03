#include <iostream>
#include <string>
#include <tmx/tmx.hpp>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <tmx_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::cout << "Testing infinite map file: " << filename << std::endl;

    // Parse the TMX file
    auto result = tmx::Parser::parseFromFile(filename);

    if (!result)
    {
        std::cerr << filename << ": FAILED - Parse error: " << result.error() << std::endl;
        return 1;
    }

    const auto& map = *result;

    // Verify this is an infinite map
    if (!map.infinite)
    {
        std::cerr << filename << ": ERROR - Expected infinite map, got finite map" << std::endl;
        return 1;
    }

    std::cout << filename << ": Map is infinite: " << (map.infinite ? "YES" : "NO") << std::endl;
    std::cout << filename << ": Map size: " << map.width << "x" << map.height << std::endl;
    std::cout << filename << ": Tile size: " << map.tilewidth << "x" << map.tileheight << std::endl;
    std::cout << filename << ": Tilesets: " << map.tilesets.size() << std::endl;
    std::cout << filename << ": Layers: " << map.layers.size() << std::endl;

    // Verify that layers have chunks
    bool hasChunks = false;
    size_t totalChunks = 0;
    size_t totalTiles = 0;

    for (const auto& layer : map.layers)
    {
        std::cout << filename << ": Layer '" << layer.name << "' has " 
                  << layer.chunks.size() << " chunks" << std::endl;
        
        if (!layer.chunks.empty())
        {
            hasChunks = true;
            totalChunks += layer.chunks.size();
            
            for (const auto& chunk : layer.chunks)
            {
                totalTiles += chunk.data.size();
            }
        }
    }

    if (!hasChunks)
    {
        std::cerr << filename << ": ERROR - No chunks found in any layer" << std::endl;
        return 1;
    }

    std::cout << filename << ": Total chunks: " << totalChunks << std::endl;
    std::cout << filename << ": Total tiles in chunks: " << totalTiles << std::endl;

    // Try to create render data
    try
    {
        auto renderData = tmx::render::createRenderData(map, "");
        std::cout << filename << ": Render data created successfully" << std::endl;
        std::cout << filename << ": Render layers: " << renderData.layers.size() << std::endl;
        
        size_t totalRenderTiles = 0;
        for (const auto& layer : renderData.layers)
        {
            totalRenderTiles += layer.tiles.size();
        }
        std::cout << filename << ": Total render tiles: " << totalRenderTiles << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << filename << ": ERROR - Failed to create render data: " << e.what() << std::endl;
        return 1;
    }

    std::cout << filename << ": PASSED - All checks successful" << std::endl;
    return 0;
}
