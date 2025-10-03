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
        bool isAnimated;             // Whether this tile has animation
        std::uint32_t animationIndex; // Index into TilesetRenderInfo::animations (-1 if not animated)
    };

    /// @brief Pre-calculated animation frame information
    struct AnimationFrameInfo
    {
        std::uint32_t tileId;        // Tile ID for this frame (after subtracting firstgid)
        std::uint32_t srcX, srcY;    // Source position in tileset (pixels)
        std::uint32_t duration;      // Duration in milliseconds
    };

    /// @brief Animation data for a specific tile
    struct TileAnimationInfo
    {
        std::uint32_t baseTileId;    // The base tile ID that has this animation
        std::vector<AnimationFrameInfo> frames;
        std::uint32_t totalDuration; // Total animation duration in milliseconds
        std::vector<std::uint32_t> timeToFrameIndex; // Flattened time-to-frame lookup (one entry per millisecond)
        
        /// @brief Get the frame index for a given time in the animation cycle
        /// @param timeInCycle Time in milliseconds within the animation cycle (0 to totalDuration-1)
        /// @return Frame index
        [[nodiscard]] inline auto getFrameIndexAtTime(std::uint32_t timeInCycle) const -> std::uint32_t
        {
            if (timeToFrameIndex.empty() || timeInCycle >= timeToFrameIndex.size())
                return 0;
            return timeToFrameIndex[timeInCycle];
        }
    };

    /// @brief Pre-calculated layer rendering information
    struct LayerRenderData
    {
        std::string name;
        bool visible;
        float opacity;
        std::vector<TileRenderInfo> tiles;  // Only non-empty tiles
    };

    /// @brief Pre-calculated object rendering information
    struct ObjectRenderInfo
    {
        std::uint32_t id;
        std::string name;
        std::string type;
        float x, y;                    // Position in pixels
        float width, height;           // Size in pixels
        float rotation;                // Rotation in degrees
        bool visible;
        map::ObjectShape shape;
        std::vector<map::Point> points; // For polygon/polyline
        std::uint32_t gid;             // For tile objects
        
        // Pre-calculated tile rendering info for tile objects (gid != 0)
        std::uint32_t tilesetIndex = static_cast<std::uint32_t>(-1);
        std::uint32_t srcX = 0, srcY = 0;
        std::uint32_t srcW = 0, srcH = 0;
    };

    /// @brief Pre-calculated object group rendering information
    struct ObjectGroupRenderData
    {
        std::string name;
        bool visible;
        float opacity;
        std::vector<ObjectRenderInfo> objects;
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
        std::vector<TileAnimationInfo> animations; // Animation data for tiles in this tileset
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
        std::vector<ObjectGroupRenderData> objectGroups;

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
