#pragma once

#include <SDL3/SDL.h>
#include <tmx/tmx.hpp>
#include <vector>
#include <string>
#include <unordered_map>

namespace tmx::sdl3 {

/// @brief Animation state tracker for a tile animation
struct AnimationState {
    uint32_t currentFrame = 0;
    uint32_t elapsedTime = 0;
};

/// @brief Animation state manager for all animations in a map
class AnimationStateManager {
public:
    /// @brief Update all animation states
    /// @param deltaTime Time elapsed since last update in milliseconds
    void update(uint32_t deltaTime);

    /// @brief Get or create animation state for a given tile
    /// @param tilesetIndex Index of the tileset
    /// @param animationIndex Index of the animation in the tileset
    /// @return Reference to the animation state
    AnimationState& getState(uint32_t tilesetIndex, uint32_t animationIndex);

private:
    std::unordered_map<uint64_t, AnimationState> states_;
};

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

/// @brief Render a single layer with animation support
/// @param renderer SDL renderer
/// @param layer Layer render data
/// @param renderData Full map render data (for animation info)
/// @param tilesetTextures Vector of loaded tileset textures
/// @param animationStates Animation state manager
/// @param deltaTime Time elapsed since last frame (for animation update)
void renderLayer(
    SDL_Renderer* renderer,
    const tmx::render::LayerRenderData& layer,
    const tmx::render::MapRenderData& renderData,
    const std::vector<SDL_Texture*>& tilesetTextures,
    AnimationStateManager& animationStates,
    uint32_t deltaTime
);

/// @brief Render all layers of a map with animation support
/// @param renderer SDL renderer
/// @param renderData Map render data
/// @param tilesetTextures Vector of loaded tileset textures
/// @param animationStates Animation state manager
/// @param deltaTime Time elapsed since last frame (for animation update)
void renderMap(
    SDL_Renderer* renderer,
    const tmx::render::MapRenderData& renderData,
    const std::vector<SDL_Texture*>& tilesetTextures,
    AnimationStateManager& animationStates,
    uint32_t deltaTime
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
