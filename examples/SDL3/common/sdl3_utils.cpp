#include "sdl3_utils.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace tmx::sdl3 {

void AnimationStateManager::update(uint32_t deltaTime) {
    for (auto& [key, state] : states_) {
        state.elapsedTime += deltaTime;
    }
}

AnimationState& AnimationStateManager::getState(uint32_t tilesetIndex, uint32_t animationIndex) {
    uint64_t key = (static_cast<uint64_t>(tilesetIndex) << 32) | animationIndex;
    return states_[key];
}

bool initSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL3: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool createWindowAndRenderer(
    const char* title,
    int width,
    int height,
    SDL_Window** outWindow,
    SDL_Renderer** outRenderer
) {
    *outWindow = SDL_CreateWindow(title, width, height, 0);
    if (!*outWindow) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    *outRenderer = SDL_CreateRenderer(*outWindow, nullptr);
    if (!*outRenderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(*outWindow);
        return false;
    }

    return true;
}

SDL_Texture* loadTilesetTexture(SDL_Renderer* renderer, const std::string& imagePath) {
    // Load image using stb_image
    int width, height, channels;
    unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height, &channels, 4);

    if (!imageData) {
        std::cerr << "Failed to load tileset image '" << imagePath << "': " << stbi_failure_reason() << std::endl;
        return nullptr;
    }

    // Create SDL surface from image data
    SDL_Surface* surface = SDL_CreateSurfaceFrom(
        width,
        height,
        SDL_PIXELFORMAT_RGBA32,
        imageData,
        width * 4
    );

    if (!surface) {
        std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
        stbi_image_free(imageData);
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    }

    // Cleanup
    SDL_DestroySurface(surface);
    stbi_image_free(imageData);

    return texture;
}

std::vector<SDL_Texture*> loadTilesetTextures(
    SDL_Renderer* renderer,
    const tmx::render::MapRenderData& renderData
) {
    std::vector<SDL_Texture*> textures;
    textures.reserve(renderData.tilesets.size());

    for (const auto& tilesetInfo : renderData.tilesets) {
        std::cout << "Loading tileset: " << tilesetInfo.imagePath << std::endl;
        SDL_Texture* texture = loadTilesetTexture(renderer, tilesetInfo.imagePath);
        textures.push_back(texture);
    }

    return textures;
}

void cleanup(
    std::vector<SDL_Texture*>& textures,
    SDL_Renderer* renderer,
    SDL_Window* window
) {
    // Destroy textures
    for (auto* texture : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    textures.clear();

    // Destroy renderer and window
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }

    // Quit SDL
    SDL_Quit();
}

void renderLayer(
    SDL_Renderer* renderer,
    const tmx::render::LayerRenderData& layer,
    const tmx::render::MapRenderData& renderData,
    const std::vector<SDL_Texture*>& tilesetTextures,
    AnimationStateManager& animationStates,
    uint32_t deltaTime
) {
    if (!layer.visible) {
        return;
    }

    for (const auto& tile : layer.tiles) {
        // Get the tileset texture
        if (tile.tilesetIndex >= tilesetTextures.size()) {
            continue;
        }

        SDL_Texture* texture = tilesetTextures[tile.tilesetIndex];
        if (!texture) {
            continue;
        }

        SDL_FRect srcRect;
        SDL_FRect destRect;

        if (tile.isAnimated && tile.animationIndex != static_cast<uint32_t>(-1)) {
            // Get animation info
            const auto& tilesetInfo = renderData.tilesets[tile.tilesetIndex];
            if (tile.animationIndex >= tilesetInfo.animations.size()) {
                continue;
            }

            const auto& animation = tilesetInfo.animations[tile.animationIndex];

            // Get or create animation state
            auto& state = animationStates.getState(tile.tilesetIndex, tile.animationIndex);

            // Update animation
            state.elapsedTime += deltaTime;

            // Use flattened lookup to get current frame index - O(1) instead of O(n)
            const uint32_t timeInCycle = state.elapsedTime % animation.totalDuration;
            const uint32_t frameIndex = animation.getFrameIndexAtTime(timeInCycle);

            // Use the current animation frame
            const auto& frame = animation.frames[frameIndex];
            srcRect = {
                static_cast<float>(frame.srcX),
                static_cast<float>(frame.srcY),
                static_cast<float>(tile.srcW),
                static_cast<float>(tile.srcH)
            };
        } else {
            // Static tile - use pre-calculated source rect
            srcRect = {
                static_cast<float>(tile.srcX),
                static_cast<float>(tile.srcY),
                static_cast<float>(tile.srcW),
                static_cast<float>(tile.srcH)
            };
        }

        // Destination is always the same
        destRect = {
            static_cast<float>(tile.destX),
            static_cast<float>(tile.destY),
            static_cast<float>(tile.destW),
            static_cast<float>(tile.destH)
        };

        // Apply opacity if not fully opaque
        if (tile.opacity < 1.0f) {
            SDL_SetTextureAlphaModFloat(texture, tile.opacity);
        }

        SDL_RenderTexture(renderer, texture, &srcRect, &destRect);

        // Reset opacity
        if (tile.opacity < 1.0f) {
            SDL_SetTextureAlphaModFloat(texture, 1.0f);
        }
    }
}

void renderMap(
    SDL_Renderer* renderer,
    const tmx::render::MapRenderData& renderData,
    const std::vector<SDL_Texture*>& tilesetTextures,
    AnimationStateManager& animationStates,
    uint32_t deltaTime
) {
    for (const auto& layer : renderData.layers) {
        renderLayer(renderer, layer, renderData, tilesetTextures, animationStates, deltaTime);
    }
}

} // namespace tmx::sdl3
