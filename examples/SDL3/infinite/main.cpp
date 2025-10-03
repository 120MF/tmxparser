#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <tmx/tmx.hpp>
#include <filesystem>
#include "../common/sdl3_utils.hpp"

int main(int argc, char* argv[])
{
    std::cout << "TMX Parser SDL3 Infinite Map Rendering Example" << std::endl;

    // Parse the infinite TMX file
    std::filesystem::path assetDir = ASSET_DIR;
    auto result = tmx::Parser::parseFromFile(assetDir / "infinite/Interior1.tmx");

    if (!result)
    {
        std::cerr << "Failed to parse TMX file: " << result.error() << std::endl;
        return 1;
    }

    const auto& map = *result;

    std::cout << "Successfully parsed infinite TMX map:" << std::endl;
    std::cout << "  Infinite: " << (map.infinite ? "YES" : "NO") << std::endl;
    std::cout << "  Size: " << map.width << "x" << map.height << " (tiles)" << std::endl;
    std::cout << "  Tile size: " << map.tilewidth << "x" << map.tileheight << " (pixels)" << std::endl;

    // Create render data (pre-calculate all tile positions from chunks)
    std::cout << "Preparing render data..." << std::endl;
    auto renderData = tmx::render::createRenderData(map, (assetDir / "infinite").string());

    std::cout << "  Tilesets: " << renderData.tilesets.size() << std::endl;
    std::cout << "  Layers: " << renderData.layers.size() << std::endl;

    // Count chunks in layers
    size_t totalChunks = 0;
    for (const auto& layer : map.layers)
    {
        totalChunks += layer.chunks.size();
    }
    std::cout << "  Total chunks: " << totalChunks << std::endl;

    size_t totalTiles = 0;
    for (const auto& layer : renderData.layers)
    {
        totalTiles += layer.tiles.size();
    }
    std::cout << "  Renderable tiles: " << totalTiles << std::endl;

    // Calculate the bounding box of all tiles for window size
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    bool first = true;

    for (const auto& layer : renderData.layers)
    {
        for (const auto& tile : layer.tiles)
        {
            int tileX = static_cast<int>(tile.destX);
            int tileY = static_cast<int>(tile.destY);
            int tileRight = tileX + static_cast<int>(tile.destW);
            int tileBottom = tileY + static_cast<int>(tile.destH);

            if (first)
            {
                minX = tileX;
                minY = tileY;
                maxX = tileRight;
                maxY = tileBottom;
                first = false;
            }
            else
            {
                minX = std::min(minX, tileX);
                minY = std::min(minY, tileY);
                maxX = std::max(maxX, tileRight);
                maxY = std::max(maxY, tileBottom);
            }
        }
    }

    const int mapPixelWidth = maxX - minX;
    const int mapPixelHeight = maxY - minY;
    const int offsetX = -minX;
    const int offsetY = -minY;

    std::cout << "  Map bounds: " << minX << "," << minY << " to " << maxX << "," << maxY << std::endl;
    std::cout << "  Map pixel size: " << mapPixelWidth << "x" << mapPixelHeight << std::endl;
    std::cout << "  Render offset: " << offsetX << "," << offsetY << std::endl;

    // Initialize SDL3
    if (!tmx::sdl3::initSDL())
    {
        return 1;
    }

    // Create window and renderer - scale down if map is too large
    constexpr int maxWindowWidth = 1280;
    constexpr int maxWindowHeight = 720;

    float scale = 1.0f;
    int windowWidth = mapPixelWidth;
    int windowHeight = mapPixelHeight;

    if (windowWidth > maxWindowWidth || windowHeight > maxWindowHeight)
    {
        float scaleX = static_cast<float>(maxWindowWidth) / windowWidth;
        float scaleY = static_cast<float>(maxWindowHeight) / windowHeight;
        scale = std::min(scaleX, scaleY);
        windowWidth = static_cast<int>(windowWidth * scale);
        windowHeight = static_cast<int>(windowHeight * scale);
        std::cout << "  Scaling to fit window: " << scale << "x" << std::endl;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!tmx::sdl3::createWindowAndRenderer(
        "TMXParser SDL3 Infinite Map Example - Interior1.tmx",
        windowWidth,
        windowHeight,
        &window,
        &renderer))
    {
        SDL_Quit();
        return 1;
    }

    // Set render scale if needed
    if (scale != 1.0f)
    {
        SDL_SetRenderScale(renderer, scale, scale);
    }

    // Load tileset textures
    auto tilesetTextures = tmx::sdl3::loadTilesetTextures(renderer, renderData);

    std::cout << "Rendering infinite map... Press ESC to quit, Arrow keys to pan." << std::endl;

    // Camera position for panning
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    const float panSpeed = 10.0f;

    // Animation state manager
    tmx::sdl3::AnimationStateManager animationStates;

    // Main loop
    bool running = true;
    SDL_Event event;
    Uint64 lastTime = SDL_GetTicks();

    while (running)
    {
        // Calculate delta time
        Uint64 currentTime = SDL_GetTicks();
        Uint32 deltaTime = static_cast<Uint32>(currentTime - lastTime);
        lastTime = currentTime;

        // Update animations
        animationStates.update(deltaTime);

        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
        }

        // Handle keyboard input for panning
        const bool* keyState = SDL_GetKeyboardState(nullptr);
        if (keyState[SDL_SCANCODE_LEFT])
        {
            cameraX -= panSpeed;
        }
        if (keyState[SDL_SCANCODE_RIGHT])
        {
            cameraX += panSpeed;
        }
        if (keyState[SDL_SCANCODE_UP])
        {
            cameraY -= panSpeed;
        }
        if (keyState[SDL_SCANCODE_DOWN])
        {
            cameraY += panSpeed;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        // Render tiles using pre-calculated render data
        for (const auto& layer : renderData.layers)
        {
            if (!layer.visible)
                continue;

            for (const auto& tile : layer.tiles)
            {
                // Get the tileset texture
                if (tile.tilesetIndex >= tilesetTextures.size())
                    continue;

                SDL_Texture* texture = tilesetTextures[tile.tilesetIndex];
                if (!texture)
                    continue;

                // Apply camera offset and map offset
                SDL_FRect srcRect = {
                    static_cast<float>(tile.srcX),
                    static_cast<float>(tile.srcY),
                    static_cast<float>(tile.srcW),
                    static_cast<float>(tile.srcH)
                };

                SDL_FRect destRect = {
                    static_cast<float>(tile.destX) + offsetX - cameraX,
                    static_cast<float>(tile.destY) + offsetY - cameraY,
                    static_cast<float>(tile.destW),
                    static_cast<float>(tile.destH)
                };

                // Handle animations
                if (tile.isAnimated && tile.animationIndex != static_cast<uint32_t>(-1))
                {
                    const auto& tilesetInfo = renderData.tilesets[tile.tilesetIndex];
                    if (tile.animationIndex < tilesetInfo.animations.size())
                    {
                        const auto& animation = tilesetInfo.animations[tile.animationIndex];
                        auto& animState = animationStates.getState(tile.tilesetIndex, tile.animationIndex);

                        // Get current frame using helper method
                        if (animation.totalDuration > 0)
                        {
                            uint32_t timeInCycle = animState.elapsedTime % animation.totalDuration;
                            uint32_t currentFrameIndex = animation.getFrameIndexAtTime(timeInCycle);

                            if (currentFrameIndex < animation.frames.size())
                            {
                                const auto& frame = animation.frames[currentFrameIndex];
                                srcRect.x = static_cast<float>(frame.srcX);
                                srcRect.y = static_cast<float>(frame.srcY);
                            }
                        }
                    }
                }

                // Apply opacity if not fully opaque
                if (tile.opacity < 1.0f)
                {
                    SDL_SetTextureAlphaModFloat(texture, tile.opacity);
                }

                SDL_RenderTexture(renderer, texture, &srcRect, &destRect);

                // Reset opacity
                if (tile.opacity < 1.0f)
                {
                    SDL_SetTextureAlphaModFloat(texture, 1.0f);
                }
            }
        }

        // Present
        SDL_RenderPresent(renderer);

        // Small delay to avoid high CPU usage
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    tmx::sdl3::cleanup(tilesetTextures, renderer, window);

    std::cout << "SDL3 infinite map example finished successfully." << std::endl;
    return 0;
}
