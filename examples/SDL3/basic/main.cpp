#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <tmx/tmx.hpp>
#include <filesystem>
#include "../common/sdl3_utils.hpp"

int main(int argc, char* argv[])
{
    std::cout << "TMX Parser SDL3 Basic Rendering Example" << std::endl;

    // Parse the TMX file
    std::filesystem::path assetDir = ASSET_DIR;
    auto result = tmx::Parser::parseFromFile(assetDir / "test.tmx");

    if (!result)
    {
        std::cerr << "Failed to parse TMX file: " << result.error() << std::endl;
        return 1;
    }

    const auto& map = *result;

    std::cout << "Successfully parsed TMX map:" << std::endl;
    std::cout << "  Size: " << map.width << "x" << map.height << std::endl;
    std::cout << "  Tile size: " << map.tilewidth << "x" << map.tileheight << std::endl;

    // Create render data (pre-calculate all tile positions)
    std::cout << "Preparing render data..." << std::endl;
    auto renderData = tmx::render::createRenderData(map, assetDir.string());

    std::cout << "  Tilesets: " << renderData.tilesets.size() << std::endl;
    std::cout << "  Layers: " << renderData.layers.size() << std::endl;

    size_t totalTiles = 0;
    for (const auto& layer : renderData.layers)
    {
        totalTiles += layer.tiles.size();
    }
    std::cout << "  Renderable tiles: " << totalTiles << std::endl;

    // Initialize SDL3
    if (!tmx::sdl3::initSDL())
    {
        return 1;
    }

    // Create window and renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    if (!tmx::sdl3::createWindowAndRenderer(
        "TMXParser SDL3 Basic Example",
        static_cast<int>(renderData.pixelWidth),
        static_cast<int>(renderData.pixelHeight),
        &window,
        &renderer))
    {
        SDL_Quit();
        return 1;
    }

    // Load tileset textures
    auto tilesetTextures = tmx::sdl3::loadTilesetTextures(renderer, renderData);

    std::cout << "Rendering map... Press ESC to quit." << std::endl;

    // Main loop
    bool running = true;
    SDL_Event event;

    while (running)
    {
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

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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

                // Use pre-calculated rectangles (no runtime calculations!)
                SDL_FRect srcRect = {
                    static_cast<float>(tile.srcX),
                    static_cast<float>(tile.srcY),
                    static_cast<float>(tile.srcW),
                    static_cast<float>(tile.srcH)
                };

                SDL_FRect destRect = {
                    static_cast<float>(tile.destX),
                    static_cast<float>(tile.destY),
                    static_cast<float>(tile.destW),
                    static_cast<float>(tile.destH)
                };

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

    std::cout << "SDL3 basic example finished successfully." << std::endl;
    return 0;
}
