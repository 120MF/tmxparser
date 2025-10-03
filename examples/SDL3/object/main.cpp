#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <cmath>
#include <tmx/tmx.hpp>
#include <filesystem>
#include "../common/sdl3_utils.hpp"

int main(int argc, char* argv[])
{
    std::cout << "TMX Parser SDL3 Object Example" << std::endl;

    // Parse the TMX file with objects
    const std::filesystem::path assetDir = ASSET_DIR;
    auto result = tmx::Parser::parseFromFile(assetDir / "island.tmx");

    if (!result)
    {
        std::cerr << "Failed to parse TMX file: " << result.error() << std::endl;
        return 1;
    }

    const auto& map = *result;

    std::cout << "Successfully parsed TMX map:" << std::endl;
    std::cout << "  Size: " << map.width << "x" << map.height << std::endl;
    std::cout << "  Tile size: " << map.tilewidth << "x" << map.tileheight << std::endl;
    std::cout << "  Object groups: " << map.objectgroups.size() << std::endl;

    // Create render data (pre-calculate all tile positions and objects)
    std::cout << "Preparing render data..." << std::endl;
    const auto renderData = tmx::render::createRenderData(map, assetDir.string());

    std::cout << "  Tilesets: " << renderData.tilesets.size() << std::endl;
    std::cout << "  Layers: " << renderData.layers.size() << std::endl;
    std::cout << "  Object groups: " << renderData.objectGroups.size() << std::endl;

    // Count objects by type
    for (const auto& objectGroup : renderData.objectGroups)
    {
        std::cout << "  Object group '" << objectGroup.name << "': " << objectGroup.objects.size() << " objects" << std::endl;
        for (const auto& object : objectGroup.objects)
        {
            std::cout << "    - " << object.name << " (" << object.type << ") at (" 
                      << object.x << ", " << object.y << ")" << std::endl;
        }
    }

    // Initialize SDL3
    if (!tmx::sdl3::initSDL())
    {
        return 1;
    }

    // Create window and renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!tmx::sdl3::createWindowAndRenderer(
        "TMXParser SDL3 Object Example",
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

    std::cout << "Rendering map with objects... Press ESC to quit." << std::endl;

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

        // Render map layers using common rendering function
        tmx::sdl3::renderMap(renderer, renderData, tilesetTextures, animationStates, deltaTime);

        // Render objects
        for (const auto& objectGroup : renderData.objectGroups)
        {
            if (!objectGroup.visible)
                continue;

            for (const auto& object : objectGroup.objects)
            {
                if (!object.visible)
                    continue;

                // Set draw color based on object type
                if (object.type == "start")
                {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 
                        static_cast<uint8_t>(255 * objectGroup.opacity)); // Green
                }
                else if (object.type == "exit")
                {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 
                        static_cast<uint8_t>(255 * objectGroup.opacity)); // Red
                }
                else if (object.type == "rest")
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 
                        static_cast<uint8_t>(255 * objectGroup.opacity)); // Blue
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 
                        static_cast<uint8_t>(255 * objectGroup.opacity)); // Yellow
                }

                // Render based on shape
                switch (object.shape)
                {
                case tmx::map::ObjectShape::Point:
                {
                    // Draw a small cross for point objects
                    const float size = 5.0f;
                    SDL_RenderLine(renderer, object.x - size, object.y, object.x + size, object.y);
                    SDL_RenderLine(renderer, object.x, object.y - size, object.x, object.y + size);
                    break;
                }
                case tmx::map::ObjectShape::Rectangle:
                {
                    SDL_FRect rect = {object.x, object.y, object.width, object.height};
                    SDL_RenderRect(renderer, &rect);
                    break;
                }
                case tmx::map::ObjectShape::Ellipse:
                {
                    // Draw ellipse as a series of line segments
                    const int segments = 32;
                    const float cx = object.x + object.width / 2.0f;
                    const float cy = object.y + object.height / 2.0f;
                    const float rx = object.width / 2.0f;
                    const float ry = object.height / 2.0f;

                    for (int i = 0; i < segments; ++i)
                    {
                        float angle1 = (2.0f * 3.14159f * i) / segments;
                        float angle2 = (2.0f * 3.14159f * (i + 1)) / segments;
                        
                        float x1 = cx + rx * std::cos(angle1);
                        float y1 = cy + ry * std::sin(angle1);
                        float x2 = cx + rx * std::cos(angle2);
                        float y2 = cy + ry * std::sin(angle2);
                        
                        SDL_RenderLine(renderer, x1, y1, x2, y2);
                    }
                    break;
                }
                case tmx::map::ObjectShape::Polygon:
                case tmx::map::ObjectShape::Polyline:
                {
                    if (object.points.size() >= 2)
                    {
                        for (size_t i = 0; i < object.points.size() - 1; ++i)
                        {
                            SDL_RenderLine(
                                renderer,
                                object.x + object.points[i].x,
                                object.y + object.points[i].y,
                                object.x + object.points[i + 1].x,
                                object.y + object.points[i + 1].y
                            );
                        }
                        
                        // Close the polygon
                        if (object.shape == tmx::map::ObjectShape::Polygon && object.points.size() > 2)
                        {
                            SDL_RenderLine(
                                renderer,
                                object.x + object.points.back().x,
                                object.y + object.points.back().y,
                                object.x + object.points[0].x,
                                object.y + object.points[0].y
                            );
                        }
                    }
                    break;
                }
                default:
                    break;
                }

                // Draw object name as text (simple representation with a point)
                if (!object.name.empty())
                {
                    // Draw a white dot to indicate there's a named object
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_FRect nameMarker = {object.x - 2, object.y - 2, 4, 4};
                    SDL_RenderFillRect(renderer, &nameMarker);
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

    std::cout << "SDL3 object example finished successfully." << std::endl;
    return 0;
}
