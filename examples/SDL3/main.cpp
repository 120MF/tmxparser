#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <tmx/tmx.hpp>
#include <filesystem>

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

    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialize SDL3: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    const auto windowWidth = map.width * map.tilewidth;
    const auto windowHeight = map.height * map.tileheight;

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

    // Load tileset texture
    SDL_Texture* tilesetTexture = nullptr;

    if (!map.tilesets.empty())
    {
        const auto& tileset = map.tilesets[0];
        const std::filesystem::path imageSource = assetDir / tileset.image;

        std::cout << "Loading tileset from: " << imageSource << std::endl;

        // Load image using stb_image
        int width, height, channels;
        unsigned char* imageData = stbi_load(imageSource.string().c_str(), &width, &height, &channels, 4);

        if (!imageData)
        {
            std::cerr << "Failed to load tileset image: " << stbi_failure_reason() << std::endl;
        }
        else
        {
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
            }
            else
            {
                tilesetTexture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_DestroySurface(surface);
                stbi_image_free(imageData);

                if (!tilesetTexture)
                {
                    std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
                }
            }
        }
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

        // Render tiles
        if (tilesetTexture && !map.layers.empty())
        {
            const auto& layer = map.layers[0];
            const auto& tileset = map.tilesets[0];

            for (std::uint32_t y = 0; y < layer.height; ++y)
            {
                for (std::uint32_t x = 0; x < layer.width; ++x)
                {
                    const std::uint32_t index = y * layer.width + x;
                    if (index >= layer.data.size())
                        continue;

                    const std::uint32_t gid = layer.data[index];
                    if (gid == 0)
                        continue; // Empty tile

                    // Calculate tile ID (subtract firstgid)
                    std::uint32_t tileId = gid - tileset.firstgid;

                    // Calculate source position in tileset
                    std::uint32_t tileX = (tileId % tileset.columns) * tileset.tilewidth;
                    std::uint32_t tileY = (tileId / tileset.columns) * tileset.tileheight;

                    // Source rectangle in tileset
                    SDL_FRect srcRect = {
                        static_cast<float>(tileX),
                        static_cast<float>(tileY),
                        static_cast<float>(tileset.tilewidth),
                        static_cast<float>(tileset.tileheight)
                    };

                    // Destination rectangle on screen
                    SDL_FRect destRect = {
                        static_cast<float>(x * map.tilewidth),
                        static_cast<float>(y * map.tileheight),
                        static_cast<float>(map.tilewidth),
                        static_cast<float>(map.tileheight)
                    };

                    SDL_RenderTexture(renderer, tilesetTexture, &srcRect, &destRect);
                }
            }
        }

        // Present
        SDL_RenderPresent(renderer);

        // Small delay to avoid high CPU usage
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    if (tilesetTexture)
        SDL_DestroyTexture(tilesetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "SDL3 example finished successfully." << std::endl;
    return 0;
}
