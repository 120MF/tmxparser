#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <tmx/tmx.hpp>
#include <filesystem>
#include "../common/sdl3_utils.hpp"

int main(int argc, char* argv[])
{
    std::cout << "TMX Parser SDL3 Animation Example" << std::endl;

    // Parse the TMX file with animations
    const std::filesystem::path assetDir = ASSET_DIR;
    auto result = tmx::Parser::parseFromFile(assetDir / "test_animation.tmx");

    if (!result)
    {
        std::cerr << "Failed to parse TMX file: " << result.error() << std::endl;
        return 1;
    }

    const auto& map = *result;

    std::cout << "Successfully parsed TMX map:" << std::endl;
    std::cout << "  Size: " << map.width << "x" << map.height << std::endl;
    std::cout << "  Tile size: " << map.tilewidth << "x" << map.tileheight << std::endl;

    // Create render data (pre-calculate all tile positions and animations)
    std::cout << "Preparing render data..." << std::endl;
    const auto renderData = tmx::render::createRenderData(map, assetDir.string());

    std::cout << "  Tilesets: " << renderData.tilesets.size() << std::endl;
    std::cout << "  Layers: " << renderData.layers.size() << std::endl;

    // Count animations
    size_t totalAnimations = 0;
    for (const auto& tileset : renderData.tilesets)
    {
        totalAnimations += tileset.animations.size();
        if (!tileset.animations.empty())
        {
            std::cout << "  Tileset '" << tileset.name << "' has " << tileset.animations.size() << " animations" <<
                std::endl;
        }
    }
    std::cout << "  Total animations: " << totalAnimations << std::endl;

    size_t totalTiles = 0;
    size_t animatedTiles = 0;
    for (const auto& layer : renderData.layers)
    {
        totalTiles += layer.tiles.size();
        for (const auto& tile : layer.tiles)
        {
            if (tile.isAnimated)
            {
                animatedTiles++;
            }
        }
    }
    std::cout << "  Renderable tiles: " << totalTiles << " (" << animatedTiles << " animated)" << std::endl;

    // Initialize SDL3
    if (!tmx::sdl3::initSDL())
    {
        return 1;
    }

    // Create window and renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!tmx::sdl3::createWindowAndRenderer(
        "TMXParser SDL3 Animation Example",
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

    // Initialize animation state manager
    tmx::sdl3::AnimationStateManager animationStates;

    std::cout << "Rendering animated map... Press ESC to quit." << std::endl;

    // Main loop
    bool running = true;
    SDL_Event event;
    uint32_t lastTime = SDL_GetTicks();

    while (running)
    {
        // Calculate delta time
        const uint32_t currentTime = SDL_GetTicks();
        const uint32_t deltaTime = currentTime - lastTime;
        lastTime = currentTime;

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

        // Render map using common rendering function
        tmx::sdl3::renderMap(renderer, renderData, tilesetTextures, animationStates, deltaTime);

        // Present
        SDL_RenderPresent(renderer);

        // Small delay to avoid high CPU usage
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    tmx::sdl3::cleanup(tilesetTextures, renderer, window);

    std::cout << "SDL3 animation example finished successfully." << std::endl;
    return 0;
}
