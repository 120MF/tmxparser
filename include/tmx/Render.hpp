#pragma once

#include <tl/expected.hpp>
#include <string>
#include <memory>
#include "Map.hpp"

// Forward declarations for when SDL3 is available
struct SDL_Renderer;
struct SDL_Texture;

namespace tmx {

class Renderer {
public:
    using Error = std::string;
    
    Renderer(SDL_Renderer* renderer);
    ~Renderer();
    
    auto loadMap(const map::Map& map) -> tl::expected<void, Error>;
    auto render(int x = 0, int y = 0) -> tl::expected<void, Error>;
    
private:
    SDL_Renderer* m_renderer;
    std::vector<SDL_Texture*> m_tilesetTextures;
    map::Map m_map;
    bool m_loaded = false;
    
    auto loadTilesetTexture(const map::Tileset& tileset) -> tl::expected<SDL_Texture*, Error>;
    auto renderLayer(const map::Layer& layer, int offsetX, int offsetY) -> tl::expected<void, Error>;
};

}
