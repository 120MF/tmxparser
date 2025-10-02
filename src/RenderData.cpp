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

            // Process animations
            for (const auto& tile : tileset.tiles)
            {
                if (!tile.animation.frames.empty())
                {
                    TileAnimationInfo animInfo;
                    animInfo.baseTileId = tile.id;
                    animInfo.totalDuration = 0;

                    for (const auto& frame : tile.animation.frames)
                    {
                        AnimationFrameInfo frameInfo;
                        frameInfo.tileId = frame.tileid;
                        frameInfo.duration = frame.duration;
                        
                        // Pre-calculate source position for this frame
                        frameInfo.srcX = (frame.tileid % tileset.columns) * tileset.tilewidth;
                        frameInfo.srcY = (frame.tileid / tileset.columns) * tileset.tileheight;
                        
                        animInfo.totalDuration += frame.duration;
                        animInfo.frames.push_back(frameInfo);
                    }

                    // Build flattened time-to-frame-index lookup table
                    // This eliminates the need for loop-based frame search at runtime
                    animInfo.timeToFrameIndex.resize(animInfo.totalDuration);
                    std::uint32_t currentTime = 0;
                    for (std::uint32_t frameIdx = 0; frameIdx < animInfo.frames.size(); ++frameIdx)
                    {
                        const auto& frame = animInfo.frames[frameIdx];
                        // Fill the lookup table for this frame's duration
                        for (std::uint32_t t = 0; t < frame.duration; ++t)
                        {
                            if (currentTime + t < animInfo.totalDuration)
                            {
                                animInfo.timeToFrameIndex[currentTime + t] = frameIdx;
                            }
                        }
                        currentTime += frame.duration;
                    }

                    tilesetInfo.animations.push_back(std::move(animInfo));
                }
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
                    
                    // Check if this tile has an animation
                    tileInfo.isAnimated = false;
                    tileInfo.animationIndex = static_cast<std::uint32_t>(-1);
                    
                    const auto& tilesetRenderInfo = renderData.tilesets[tilesetIndex];
                    for (std::uint32_t animIdx = 0; animIdx < tilesetRenderInfo.animations.size(); ++animIdx)
                    {
                        if (tilesetRenderInfo.animations[animIdx].baseTileId == tileId)
                        {
                            tileInfo.isAnimated = true;
                            tileInfo.animationIndex = animIdx;
                            break;
                        }
                    }

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
