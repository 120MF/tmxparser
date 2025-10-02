#include "sdl3_utils.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace tmx::sdl3 {

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

} // namespace tmx::sdl3
