#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "Map.hpp"

namespace tmx::render
{
    /// @brief Pre-calculated tile information for efficient rendering
    /// This structure eliminates the need for runtime calculations during rendering
    struct TileRenderInfo
    {
        std::uint32_t tileId;        // Tile ID after subtracting firstgid
        std::uint32_t srcX, srcY;    // Source position in tileset (pixels)
        std::uint32_t srcW, srcH;    // Source dimensions in tileset (pixels)
        std::uint32_t destX, destY;  // Destination position on screen (pixels)
        std::uint32_t destW, destH;  // Destination dimensions on screen (pixels)
        std::uint32_t tilesetIndex;  // Which tileset this tile belongs to
        float opacity;               // Layer opacity (0.0 - 1.0)
    };

    /// @brief Pre-calculated layer rendering information
    struct LayerRenderData
    {
        std::string name;
        bool visible;
        float opacity;
        std::vector<TileRenderInfo> tiles;  // Only non-empty tiles
    };

    /// @brief Tileset information for texture loading
    struct TilesetRenderInfo
    {
        std::string name;
        std::string imagePath;
        std::uint32_t imageWidth;
        std::uint32_t imageHeight;
        std::uint32_t firstgid;
        std::uint32_t tileWidth;
        std::uint32_t tileHeight;
        std::uint32_t columns;
        std::uint32_t tileCount;
    };

    /// @brief Complete rendering data for a map
    /// All tile coordinates and positions are pre-calculated for maximum performance
    struct MapRenderData
    {
        std::uint32_t mapWidth;      // Map width in tiles
        std::uint32_t mapHeight;     // Map height in tiles
        std::uint32_t tileWidth;     // Tile width in pixels
        std::uint32_t tileHeight;    // Tile height in pixels
        std::uint32_t pixelWidth;    // Total map width in pixels
        std::uint32_t pixelHeight;   // Total map height in pixels

        std::vector<TilesetRenderInfo> tilesets;
        std::vector<LayerRenderData> layers;

        /// @brief Create render data from a parsed TMX map
        /// @param map The parsed TMX map
        /// @param assetBasePath Optional base path for resolving relative tileset image paths
        /// @return MapRenderData with pre-calculated rendering information
        static auto fromMap(const map::Map& map, const std::string& assetBasePath = "") -> MapRenderData;
    };

    /// @brief Helper function to create render data from a map
    /// @param map The parsed TMX map
    /// @param assetBasePath Optional base path for resolving relative tileset image paths
    /// @return MapRenderData with pre-calculated rendering information
    inline auto createRenderData(const map::Map& map, const std::string& assetBasePath = "") -> MapRenderData
    {
        return MapRenderData::fromMap(map, assetBasePath);
    }
}
