#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <tmx/tmx.hpp>

// Expected values for all test files (they all contain the same map data)
const std::uint32_t EXPECTED_WIDTH = 10;
const std::uint32_t EXPECTED_HEIGHT = 10;
const std::uint32_t EXPECTED_TILE_WIDTH = 32;
const std::uint32_t EXPECTED_TILE_HEIGHT = 32;
const std::uint32_t EXPECTED_TILE_COUNT = 100; // 10x10
const std::string EXPECTED_LAYER_NAME = "ground";
const std::string EXPECTED_TILESET_NAME = "test_tileset";
const std::uint32_t EXPECTED_FIRST_GID = 1;

// The expected tile data (same for all test files)
const std::vector<std::uint32_t> EXPECTED_TILES = {
    1, 2, 1, 2, 1, 2, 1, 2, 1, 4,
    2, 1, 2, 1, 2, 1, 2, 1, 4, 1,
    1, 2, 1, 2, 1, 2, 1, 4, 1, 3,
    2, 1, 2, 1, 2, 1, 4, 1, 3, 1,
    1, 4, 1, 4, 1, 4, 1, 4, 1, 4,
    2, 1, 2, 1, 4, 1, 3, 1, 3, 1,
    1, 2, 1, 4, 1, 3, 1, 3, 1, 3,
    2, 1, 4, 1, 3, 1, 3, 1, 3, 1,
    1, 4, 1, 3, 1, 3, 1, 3, 1, 3,
    4, 1, 3, 1, 3, 1, 3, 1, 3, 1
};

bool verifyMap(const tmx::map::Map& map, const std::string& filename) {
    bool success = true;
    
    // Check map dimensions
    if (map.width != EXPECTED_WIDTH) {
        std::cerr << filename << ": ERROR - Expected width " << EXPECTED_WIDTH 
                  << ", got " << map.width << std::endl;
        success = false;
    }
    
    if (map.height != EXPECTED_HEIGHT) {
        std::cerr << filename << ": ERROR - Expected height " << EXPECTED_HEIGHT 
                  << ", got " << map.height << std::endl;
        success = false;
    }
    
    if (map.tilewidth != EXPECTED_TILE_WIDTH) {
        std::cerr << filename << ": ERROR - Expected tile width " << EXPECTED_TILE_WIDTH 
                  << ", got " << map.tilewidth << std::endl;
        success = false;
    }
    
    if (map.tileheight != EXPECTED_TILE_HEIGHT) {
        std::cerr << filename << ": ERROR - Expected tile height " << EXPECTED_TILE_HEIGHT 
                  << ", got " << map.tileheight << std::endl;
        success = false;
    }
    
    // Check tileset
    if (map.tilesets.empty()) {
        std::cerr << filename << ": ERROR - No tilesets found" << std::endl;
        success = false;
    } else {
        const auto& tileset = map.tilesets[0];
        if (tileset.name != EXPECTED_TILESET_NAME) {
            std::cerr << filename << ": ERROR - Expected tileset name '" << EXPECTED_TILESET_NAME 
                      << "', got '" << tileset.name << "'" << std::endl;
            success = false;
        }
        if (tileset.firstgid != EXPECTED_FIRST_GID) {
            std::cerr << filename << ": ERROR - Expected firstgid " << EXPECTED_FIRST_GID 
                      << ", got " << tileset.firstgid << std::endl;
            success = false;
        }
    }
    
    // Check layers
    if (map.layers.empty()) {
        std::cerr << filename << ": ERROR - No layers found" << std::endl;
        success = false;
    } else {
        const auto& layer = map.layers[0];
        if (layer.name != EXPECTED_LAYER_NAME) {
            std::cerr << filename << ": ERROR - Expected layer name '" << EXPECTED_LAYER_NAME 
                      << "', got '" << layer.name << "'" << std::endl;
            success = false;
        }
        
        if (layer.data.size() != EXPECTED_TILE_COUNT) {
            std::cerr << filename << ": ERROR - Expected " << EXPECTED_TILE_COUNT 
                      << " tiles, got " << layer.data.size() << std::endl;
            success = false;
        } else {
            // Check tile data
            for (size_t i = 0; i < EXPECTED_TILES.size(); ++i) {
                if (layer.data[i] != EXPECTED_TILES[i]) {
                    std::cerr << filename << ": ERROR - Tile mismatch at index " << i 
                              << ": expected " << EXPECTED_TILES[i] 
                              << ", got " << layer.data[i] << std::endl;
                    success = false;
                    break;
                }
            }
        }
    }
    
    if (success) {
        std::cout << filename << ": PASSED - All checks successful" << std::endl;
    }
    
    return success;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <tmx_file>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    
    std::cout << "Testing file: " << filename << std::endl;
    
    // Parse the TMX file
    auto result = tmx::Parser::parseFromFile(filename);
    
    if (!result) {
        std::cerr << filename << ": FAILED - Parse error: " << result.error() << std::endl;
        return 1;
    }
    
    const auto& map = *result;
    
    // Verify the parsed data
    if (!verifyMap(map, filename)) {
        return 1;
    }
    
    return 0;
}
