#pragma once

#include <SDL3/SDL.h>
#include <tmx/tmx.hpp>
#include <vector>
#include <string>

namespace tmx::sdl3 {

/// @brief Initialize SDL3 subsystems
/// @return true on success, false on failure
bool initSDL();

/// @brief Create an SDL3 window and renderer
/// @param title Window title
/// @param width Window width in pixels
/// @param height Window height in pixels
/// @param outWindow Output parameter for the created window
/// @param outRenderer Output parameter for the created renderer
/// @return true on success, false on failure
bool createWindowAndRenderer(
    const char* title,
    int width,
    int height,
    SDL_Window** outWindow,
    SDL_Renderer** outRenderer
);

/// @brief Load a tileset texture from a file
/// @param renderer SDL renderer
/// @param imagePath Path to the image file
/// @return SDL_Texture pointer, or nullptr on failure
SDL_Texture* loadTilesetTexture(SDL_Renderer* renderer, const std::string& imagePath);

/// @brief Load all tileset textures for render data
/// @param renderer SDL renderer
/// @param renderData Map render data containing tileset information
/// @return Vector of SDL_Texture pointers (may contain nullptr for failed loads)
std::vector<SDL_Texture*> loadTilesetTextures(
    SDL_Renderer* renderer,
    const tmx::render::MapRenderData& renderData
);

/// @brief Cleanup SDL resources
/// @param textures Vector of textures to destroy
/// @param renderer Renderer to destroy
/// @param window Window to destroy
void cleanup(
    std::vector<SDL_Texture*>& textures,
    SDL_Renderer* renderer,
    SDL_Window* window
);

} // namespace tmx::sdl3
