#include <tmx/RenderData.hpp>
#include <filesystem>

namespace tmx::render
{
    auto MapRenderData::fromMap(const map::Map& map, const std::string& assetBasePath) -> MapRenderData
    {
        MapRenderData renderData;

        // Store basic map information
        renderData.mapWidth = map.width;
        renderData.mapHeight = map.height;
        renderData.tileWidth = map.tilewidth;
        renderData.tileHeight = map.tileheight;
        renderData.pixelWidth = map.width * map.tilewidth;
        renderData.pixelHeight = map.height * map.tileheight;

        // Process tilesets
        renderData.tilesets.reserve(map.tilesets.size());
        for (const auto& tileset : map.tilesets)
        {
            TilesetRenderInfo tilesetInfo;
            tilesetInfo.name = tileset.name;
            tilesetInfo.imageWidth = tileset.imagewidth;
            tilesetInfo.imageHeight = tileset.imageheight;
            tilesetInfo.firstgid = tileset.firstgid;
            tilesetInfo.tileWidth = tileset.tilewidth;
            tilesetInfo.tileHeight = tileset.tileheight;
            tilesetInfo.columns = tileset.columns;
            tilesetInfo.tileCount = tileset.tilecount;

            // Resolve image path
            if (!assetBasePath.empty() && !tileset.image.empty())
            {
                std::filesystem::path basePath(assetBasePath);
                std::filesystem::path imagePath(tileset.image);
                tilesetInfo.imagePath = (basePath / imagePath).string();
            }
            else
            {
                tilesetInfo.imagePath = tileset.image;
            }

            renderData.tilesets.push_back(std::move(tilesetInfo));
        }

        // Process layers
        renderData.layers.reserve(map.layers.size());
        for (const auto& layer : map.layers)
        {
            LayerRenderData layerData;
            layerData.name = layer.name;
            layerData.visible = layer.visible;
            layerData.opacity = layer.opacity;

            // Pre-calculate all tile rendering information
            // Reserve space for worst case (all tiles non-empty)
            layerData.tiles.reserve(layer.data.size());

            for (std::uint32_t y = 0; y < layer.height; ++y)
            {
                for (std::uint32_t x = 0; x < layer.width; ++x)
                {
                    const std::uint32_t index = y * layer.width + x;
                    if (index >= layer.data.size())
                        continue;

                    const std::uint32_t gid = layer.data[index];
                    if (gid == 0)
                        continue; // Skip empty tiles

                    // Find which tileset this tile belongs to
                    std::uint32_t tilesetIndex = 0;
                    const map::Tileset* tileset = nullptr;

                    for (std::uint32_t i = 0; i < map.tilesets.size(); ++i)
                    {
                        if (gid >= map.tilesets[i].firstgid)
                        {
                            // Check if this is the right tileset
                            if (i + 1 >= map.tilesets.size() || gid < map.tilesets[i + 1].firstgid)
                            {
                                tilesetIndex = i;
                                tileset = &map.tilesets[i];
                                break;
                            }
                        }
                    }

                    if (!tileset)
                        continue; // Invalid tile

                    // Calculate tile ID (subtract firstgid)
                    const std::uint32_t tileId = gid - tileset->firstgid;

                    // Pre-calculate source position in tileset
                    const std::uint32_t tileX = (tileId % tileset->columns) * tileset->tilewidth;
                    const std::uint32_t tileY = (tileId / tileset->columns) * tileset->tileheight;

                    // Pre-calculate destination position on screen
                    const std::uint32_t destX = x * map.tilewidth;
                    const std::uint32_t destY = y * map.tileheight;

                    // Create tile render info
                    TileRenderInfo tileInfo{};
                    tileInfo.tileId = tileId;
                    tileInfo.srcX = tileX;
                    tileInfo.srcY = tileY;
                    tileInfo.srcW = tileset->tilewidth;
                    tileInfo.srcH = tileset->tileheight;
                    tileInfo.destX = destX;
                    tileInfo.destY = destY;
                    tileInfo.destW = map.tilewidth;
                    tileInfo.destH = map.tileheight;
                    tileInfo.tilesetIndex = tilesetIndex;
                    tileInfo.opacity = layer.opacity;

                    layerData.tiles.push_back(std::move(tileInfo));
                }
            }

            // Shrink to fit to save memory
            layerData.tiles.shrink_to_fit();
            renderData.layers.push_back(std::move(layerData));
        }

        return renderData;
    }
}
