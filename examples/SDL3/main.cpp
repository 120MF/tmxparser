#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <tmx/tmx.hpp>
#include <filesystem>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char* argv[])
{
    std::cout << "TMX Parser SDL3 Rendering Example" << std::endl;

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
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialize SDL3: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    const int windowWidth = static_cast<int>(renderData.pixelWidth);
    const int windowHeight = static_cast<int>(renderData.pixelHeight);

    SDL_Window* window = SDL_CreateWindow(
        "TMXParser SDL3 Example",
        windowWidth,
        windowHeight,
        0
    );

    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load tileset textures
    std::vector<SDL_Texture*> tilesetTextures;
    tilesetTextures.reserve(renderData.tilesets.size());

    for (const auto& tilesetInfo : renderData.tilesets)
    {
        std::cout << "Loading tileset: " << tilesetInfo.imagePath << std::endl;

        // Load image using stb_image
        int width, height, channels;
        unsigned char* imageData = stbi_load(tilesetInfo.imagePath.c_str(), &width, &height, &channels, 4);

        if (!imageData)
        {
            std::cerr << "Failed to load tileset image: " << stbi_failure_reason() << std::endl;
            tilesetTextures.push_back(nullptr);
            continue;
        }

        // Create SDL surface from image data
        SDL_Surface* surface = SDL_CreateSurfaceFrom(
            width, height,
            SDL_PIXELFORMAT_RGBA32,
            imageData,
            width * 4
        );

        if (!surface)
        {
            std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
            stbi_image_free(imageData);
            tilesetTextures.push_back(nullptr);
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        stbi_image_free(imageData);

        if (!texture)
        {
            std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
            tilesetTextures.push_back(nullptr);
            continue;
        }

        tilesetTextures.push_back(texture);
    }

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

            // Set layer opacity if needed
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
    for (auto* texture : tilesetTextures)
    {
        if (texture)
            SDL_DestroyTexture(texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "SDL3 example finished successfully." << std::endl;
    return 0;
}
