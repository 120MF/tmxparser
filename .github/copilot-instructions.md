# TMXParser 项目 - GitHub Copilot 开发指南

## 项目概述

TMXParser 是一个现代化的 C++ TMX（Tiled Map Exchange）地图文件解析库，专门为游戏开发而设计。该项目采用最新的 C++23
标准，提供高性能、缓存友好的地图解析功能。

### 核心特性

- **现代 C++23** - 利用最新语言特性提供类型安全和性能优化
- **TMX 标准支持** - 完全遵循 [Tiled Map Editor 官方规范](https://doc.mapeditor.org/en/stable/reference/tmx-map-format/)
- **纯解析库** - 专注于 TMX 格式解析，渲染由用户在应用层实现
- **渲染无关数据结构** - 提供平台无关的预计算渲染数据，消除渲染循环中的重复计算
- **并发与协程** - 异步地图加载和处理能力
- **SIMD 优化** - 利用向量指令加速数据处理
- **错误处理** - 基于 `tl::expected` 的现代错误处理模式
- **缓存友好设计** - 优化内存布局以提高性能

## 架构设计

### 目录结构

```
tmxparser/
├── include/tmx/          # 公共头文件
│   ├── tmx.hpp          # 主入口头文件
│   ├── Map.hpp          # TMX 数据结构定义
│   ├── Parser.hpp       # 解析器接口
│   └── RenderData.hpp   # 渲染数据结构
├── src/                 # 源文件实现
│   ├── Map.cpp
│   ├── Parser.cpp
│   └── RenderData.cpp
├── examples/            # 示例代码
│   ├── basic/          # 基础使用示例
│   └── SDL3/           # SDL3 渲染示例
│       ├── basic/      # 基础渲染示例
│       ├── animated/   # 动画渲染示例
│       └── common/     # 公共SDL3工具库
├── tests/              # 单元测试（基于 CTest）
│   ├── CMakeLists.txt # 测试配置
│   └── test_parser.cpp # 解析器测试
├── assets/             # 测试资源文件
│   ├── test.tmx       # CSV 编码
│   ├── test_b64.tmx   # Base64 编码
│   ├── test_b64_gzip.tmx  # Base64 + gzip
│   ├── test_b64_zlib.tmx  # Base64 + zlib
│   ├── test_b64_zstd.tmx  # Base64 + zstd
│   ├── test_animation.tmx # 动画测试地图
│   ├── fire_animation.tsx # 外部瓦片集
│   ├── fire_animation.png # 动画瓦片集图像
│   └── test_tileset.png   # 测试瓦片集
├── .github/
│   └── workflows/
│       └── ci.yml      # GitHub Actions CI 配置
└── cmake/              # CMake 构建脚本
```

### 核心组件

#### 1. TMX 数据结构 (`include/tmx/Map.hpp`)

```cpp
// 所有TMX数据结构位于 tmx::map 命名空间中：
namespace tmx::map {
    struct Map;          // 地图主结构
    struct Layer;        // 图层数据
    struct Tileset;      // 瓦片集
    struct Tile;         // 瓦片（带动画或属性）
    struct Animation;    // 瓦片动画
    struct Frame;        // 动画帧
    struct Properties;   // 属性系统
    struct Color;        // 颜色处理
}
```

#### 2. 解析器 (`include/tmx/Parser.hpp`)

```cpp
namespace tmx {
class Parser {
public:
    static auto parseFromFile(const std::filesystem::path& path) 
        -> tl::expected<map::Map, std::string>;
    static auto parseFromString(const std::string& xml) 
        -> tl::expected<map::Map, std::string>;
};
}
```

#### 3. 渲染数据结构 (`include/tmx/RenderData.hpp`)

```cpp
namespace tmx::render {
    // 预计算的瓦片渲染信息
    struct TileRenderInfo {
        uint32_t tileId;        // 瓦片ID
        uint32_t srcX, srcY;    // 源位置（像素）
        uint32_t srcW, srcH;    // 源尺寸
        uint32_t destX, destY;  // 目标位置
        uint32_t destW, destH;  // 目标尺寸
        uint32_t tilesetIndex;  // 所属瓦片集索引
        float opacity;          // 不透明度
        bool isAnimated;        // 是否有动画
        uint32_t animationIndex; // 动画索引
    };

    // 动画帧信息
    struct AnimationFrameInfo {
        uint32_t tileId;    // 帧瓦片ID
        uint32_t srcX, srcY; // 预计算的源位置
        uint32_t duration;  // 帧持续时间（毫秒）
    };

    // 瓦片动画信息
    struct TileAnimationInfo {
        uint32_t baseTileId;                    // 基础瓦片ID
        std::vector<AnimationFrameInfo> frames; // 动画帧
        uint32_t totalDuration;                 // 总持续时间
    };

    // 图层渲染数据
    struct LayerRenderData {
        std::string name;
        bool visible;
        float opacity;
        std::vector<TileRenderInfo> tiles;  // 仅包含非空瓦片
    };

    // 完整地图渲染数据
    struct MapRenderData {
        uint32_t mapWidth, mapHeight;      // 地图尺寸（瓦片）
        uint32_t tileWidth, tileHeight;    // 瓦片尺寸（像素）
        uint32_t pixelWidth, pixelHeight;  // 地图尺寸（像素）
        std::vector<TilesetRenderInfo> tilesets;
        std::vector<LayerRenderData> layers;
        
        // 从解析的地图创建渲染数据
        static auto fromMap(const map::Map& map, const std::string& assetBasePath = "") 
            -> MapRenderData;
    };

    // 辅助函数
    auto createRenderData(const map::Map& map, const std::string& assetBasePath = "") 
        -> MapRenderData;
}
```

> **注意**: 渲染功能已在 examples/SDL3 中使用 SDL3 实现，库本身不包含渲染器。

## 开发准则

### 1. 编码标准

- **C++23 标准**：使用最新语言特性（concepts, ranges, coroutines）
- **内存管理**：优先使用智能指针，避免裸指针
- **异常安全**：使用 `tl::expected` 进行错误处理
- **const 正确性**：确保不可变数据的正确声明
- **命名约定**：
    - 类名：PascalCase (e.g., `TilesetManager`)
    - 函数名：snake_case 或 camelCase (e.g., `parseFromFile`)
    - 成员变量：m_ 前缀 (e.g., `m_tilesets`)
    - 常量：UPPER_SNAKE_CASE (e.g., `MAX_LAYERS`)

### 2. 性能优化

- **SIMD 指令**：在数据密集型操作中使用向量化
- **缓存友好**：
    - 数据结构按照访问模式排列
    - 避免指针追逐，使用连续内存布局
    - 预分配容器大小以减少重分配
- **并发处理**：
    - 使用 C++20 协程进行异步 I/O
    - 多线程瓦片加载和解压缩
    - 无锁数据结构用于高频访问场景

### 3. 错误处理模式

```cpp
// 使用 tl::expected 替代异常
auto parseLayer(const pugi::xml_node& node) -> tl::expected<map::Layer, std::string> {
    if (!node) {
        return tl::make_unexpected("Invalid node");
    }
    // ... 解析逻辑
    return layer;
}
```

## TMX 格式支持

### 已实现功能

- ✅ 基础地图属性（尺寸、瓦片大小、方向）
- ✅ 瓦片集管理（firstgid、图像路径、瓦片尺寸）
- ✅ 图层数据（CSV 和 Base64 编码）
- ✅ 完整压缩支持（zlib、gzip、zstd）
- ✅ 属性系统（键值对）
- ✅ 颜色解析（十六进制格式）
- ✅ 瓦片动画 (Tile Animations)
- ✅ 外部瓦片集 (.tsx 文件)

### 待实现功能

- ⭕ 对象层 (Object Layers)
- ⭕ 图像层 (Image Layers)
- ⭕ 组图层 (Group Layers)
- ⭕ 地形类型 (Terrain Types)
- ⭕ 无限地图 (Infinite Maps)
- ⭕ 六边形和等距地图支持

## 渲染集成

TMXParser 是一个纯粹的解析库，不包含渲染功能。渲染应该在应用层实现。

### SDL3 渲染示例

项目在 `examples/SDL3` 目录下提供了完整的 SDL3 渲染示例实现，包含：

1. **Basic (基础渲染示例)**：
   - 位于 `examples/SDL3/basic/`
   - 展示静态地图的基础渲染
   - 使用预计算渲染数据
   - 支持多图层和透明度

2. **Animated (动画渲染示例)**：
   - 位于 `examples/SDL3/animated/`
   - 展示外部 TSX 瓦片集解析
   - 实现瓦片动画系统
   - 基于时间的帧更新
   - 多个动画同时运行

3. **Common (公共库)**：
   - 位于 `examples/SDL3/common/`
   - 提供 SDL3 工具函数
   - 纹理加载和资源管理
   - 所有示例共享的代码

#### 运行 SDL3 示例

```bash
# 构建项目（需要启用示例构建选项）
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TMX_EXAMPLES=ON
make -j$(nproc)

# 运行基础示例
./examples/SDL3/basic/tmxparser_sdl3_basic

# 运行动画示例
./examples/SDL3/animated/tmxparser_sdl3_animated
```

#### 渲染核心代码片段

```cpp
// 1. 解析TMX文件
auto result = tmx::Parser::parseFromFile("map.tmx");
const auto& map = *result;

// 2. 创建渲染数据（预计算所有瓦片位置）
auto renderData = tmx::render::createRenderData(map, assetDir);

// 3. 加载瓦片集纹理（使用 stb_image）
std::vector<SDL_Texture*> tilesetTextures;
for (const auto& tilesetInfo : renderData.tilesets) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(tilesetInfo.imagePath.c_str(), &width, &height, &channels, 4);
    
    SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, imageData, width * 4);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    stbi_image_free(imageData);
    
    tilesetTextures.push_back(texture);
}

// 4. 渲染循环 - 使用预计算的数据，无需运行时计算！
for (const auto& layer : renderData.layers) {
    if (!layer.visible) continue;
    
    for (const auto& tile : layer.tiles) {
        SDL_Texture* texture = tilesetTextures[tile.tilesetIndex];
        
        // 直接使用预计算的矩形，无需运行时计算！
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
        
        SDL_RenderTexture(renderer, texture, &srcRect, &destRect);
    }
}
```

**性能优势：**

- ✅ 无需每帧计算 `tileId = gid - firstgid`
- ✅ 无需每帧计算 `tileX = (tileId % columns) * tilewidth`
- ✅ 无需每帧计算 `tileY = (tileId / columns) * tileheight`
- ✅ 无需每帧判断瓦片所属哪个瓦片集
- ✅ 仅遍历非空瓦片，跳过所有空瓦片
- ✅ 数据紧凑，缓存友好

```

#### 依赖项

SDL3 示例需要以下依赖：
- **SDL3**: 通过 CMake FetchContent 自动下载和构建
- **stb_image**: 单头文件库，用于加载 PNG/JPG 等格式图像
- **X11 开发库** (Linux): `libx11-dev`, `libxext-dev` 等

**注意：** 需要在 CMake 配置时添加 `-DBUILD_TMX_EXAMPLES=ON` 选项以构建示例。

#### 渲染优化建议

对于生产环境，建议进一步优化：
   - 使用纹理图集减少绘制调用
   - 实现视锥裁剪只渲染可见区域
   - 使用批量渲染提高性能
   - 缓存瓦片纹理避免重复加载

## 构建系统

### CMake 配置
项目使用现代 CMake (≥3.20) 构建：

```cmake
# 依赖管理通过 FetchContent
- pugixml      # XML 解析
- tl::expected # 错误处理
- base64       # Base64 解码
- zlib         # 数据解压缩
- zstd         # 高效压缩
- SDL3         # 图形渲染（仅示例使用）
- stb_image    # 图像加载（仅示例使用）
```

### 构建步骤

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TMX_EXAMPLES=ON
make -j$(nproc)
```

**CMake 选项：**

- `BUILD_TMX_EXAMPLES=ON`: 构建示例程序（basic 和 SDL3）
- `BUILD_TMX_TESTS=ON`: 构建单元测试

### 平台支持

- **Linux**: 主要开发平台，完全支持
- **Windows**: 计划支持，需要 Visual Studio 2022
- **macOS**: 计划支持，需要 Xcode 15+

## 测试策略

### 单元测试（基于 CTest）

项目使用 CMake CTest 进行单元测试。测试位于 `tests/` 目录下。

#### 运行测试

```bash
# 构建项目并启用测试
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TMX_TESTS=ON
make -j$(nproc)

# 运行所有测试
ctest --output-on-failure

# 运行特定测试
ctest -R test_csv --verbose
```

#### 测试覆盖

当前测试涵盖所有支持的数据编码和压缩格式：

1. **test_csv** - CSV 编码测试
2. **test_base64** - Base64 编码（无压缩）测试
3. **test_base64_gzip** - Base64 + gzip 压缩测试
4. **test_base64_zlib** - Base64 + zlib 压缩测试
5. **test_base64_zstd** - Base64 + zstd 压缩测试

每个测试验证：

- 地图尺寸正确性
- 瓦片集信息正确性
- 图层数据完整性
- 瓦片数据一致性

#### 持续集成

项目使用 GitHub Actions 进行自动化测试：

- 每次推送到 `master` 分支时自动运行
- 每个 Pull Request 都会触发测试
- 测试结果会显示在 PR 检查中

#### 添加新测试

在 `tests/CMakeLists.txt` 中添加新测试：

```cmake
add_test(NAME test_name
        COMMAND test_parser "${PROJECT_SOURCE_DIR}/assets/test_file.tmx"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
```

### 性能测试

- 解析器性能基准测试（计划中）
- 内存泄漏检测（计划中）
- 边界条件验证（计划中）

### 集成测试

- 与实际游戏引擎集成测试（计划中）
- 大型地图性能测试（计划中）
- 多线程安全性验证（计划中）

## 示例用法

### 基础解析

```cpp
#include <tmx/tmx.hpp>

int main() {
    auto result = tmx::Parser::parseFromFile("map.tmx");
    if (!result) {
        std::cerr << "解析失败: " << result.error() << std::endl;
        return 1;
    }
    
    const auto& map = *result;
    std::cout << "地图大小: " << map.width << "x" << map.height << std::endl;
    std::cout << "瓦片大小: " << map.tilewidth << "x" << map.tileheight << std::endl;
    
    return 0;
}
```

### SDL3 渲染集成示例

项目在 `examples/SDL3` 目录提供了完整的 SDL3 渲染实现。以下是简化的代码结构：

```cpp
#include <tmx/tmx.hpp>
#include <SDL3/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
    // 1. 解析地图
    auto result = tmx::Parser::parseFromFile("test.tmx");
    if (!result) {
        std::cerr << "解析失败: " << result.error() << std::endl;
        return 1;
    }
    const auto& map = *result;
    
    // 2. 创建渲染数据（一次性预计算所有瓦片位置）
    auto renderData = tmx::render::createRenderData(map, assetDir);
    
    // 初始化 SDL3
    SDL_Init(SDL_INIT_VIDEO);
    auto window = SDL_CreateWindow(
        "TMXParser SDL3 Example",
        renderData.pixelWidth,
        renderData.pixelHeight,
        0
    );
    auto renderer = SDL_CreateRenderer(window, nullptr);
    
    // 3. 使用 stb_image 加载瓦片集纹理
    std::vector<SDL_Texture*> tilesetTextures;
    for (const auto& tilesetInfo : renderData.tilesets) {
        int width, height, channels;
        unsigned char* imageData = stbi_load(tilesetInfo.imagePath.c_str(), &width, &height, &channels, 4);
        
        auto surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, imageData, width * 4);
        auto texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        stbi_image_free(imageData);
        
        tilesetTextures.push_back(texture);
    }
    
    // 4. 主渲染循环
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || 
                (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                running = false;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // 渲染瓦片 - 使用预计算数据，无运行时计算！
        for (const auto& layer : renderData.layers) {
            if (!layer.visible) continue;
            
            for (const auto& tile : layer.tiles) {
                SDL_Texture* texture = tilesetTextures[tile.tilesetIndex];
                
                SDL_FRect srcRect = {
                    static_cast<float>(tile.srcX), static_cast<float>(tile.srcY),
                    static_cast<float>(tile.srcW), static_cast<float>(tile.srcH)
                };
                SDL_FRect destRect = {
                    static_cast<float>(tile.destX), static_cast<float>(tile.destY),
                    static_cast<float>(tile.destW), static_cast<float>(tile.destH)
                };
                
                if (tile.opacity < 1.0f) {
                    SDL_SetTextureAlphaModFloat(texture, tile.opacity);
                }
                
                SDL_RenderTexture(renderer, texture, &srcRect, &destRect);
                
                if (tile.opacity < 1.0f) {
                    SDL_SetTextureAlphaModFloat(texture, 1.0f);
                }
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }
    
    // 清理
    for (auto* texture : tilesetTextures) {
        SDL_DestroyTexture(texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
```

> **提示**: 完整的实现请参考 `examples/SDL3/main.cpp`，包含了错误处理和更详细的注释。

**关键优势：**

- 使用 `tmx::render::createRenderData()` 创建渲染数据结构
- 所有瓦片位置在渲染前一次性计算完毕
- 渲染循环中无需任何数学计算，直接使用预计算值
- 自动跳过空瓦片，仅包含需要渲染的瓦片
- 支持多瓦片集、图层透明度和可见性

## 未来发展方向

### 短期目标 (1-2 个月)

1. 完善基础 TMX 功能支持
2. ✅ 提供完整的 SDL3 渲染示例
3. ✅ 添加完整的单元测试套件（CTest + 5种编码/压缩格式测试）
4. ✅ 集成 GitHub Actions CI/CD
5. 性能优化和基准测试

### 中期目标 (3-6 个月)

1. 支持所有 TMX 特性
2. 多线程和协程优化
3. SIMD 指令优化
4. 跨平台支持完善

### 长期目标 (6+ 个月)

1. 游戏引擎集成插件
2. 编辑器工具支持
3. 云端地图流送
4. VR/AR 渲染支持

## 贡献指南

### 代码审查清单

- [ ] 遵循项目编码规范
- [ ] 包含适当的单元测试
- [ ] 性能无明显回退
- [ ] 内存安全检查通过
- [ ] 文档更新完整

### 优先开发领域

1. **高优先级**：基础 TMX 功能补全
2. **中优先级**：性能优化和渲染增强
3. **低优先级**：高级特性和工具集成

## 技术债务管理

### 当前已知问题

1. 错误处理需要更细粒度的错误类型
2. 内存池分配器待实现
3. 日志系统缺失
4. 需要更多单元测试覆盖

### 重构计划

1. 插件化架构设计
2. 配置系统标准化
3. 资源管理生命周期优化
4. 提供渲染示例实现

---

## 快速上手 (新开发者)

1. **环境准备**：
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake libx11-dev libxext-dev
   
   # 克隆并构建
   git clone <repository>
   cd tmxparser
   mkdir build && cd build
   cmake .. && make
   ```

2. **运行示例**：
   ```bash
   ./examples/basic/tmxparser_basic_example
   ```

3. **阅读源码**：
    - 从 `include/tmx/tmx.hpp` 开始
    - 查看 `examples/basic/main.cpp` 了解用法
    - 深入 `src/Parser/Parser.cpp` 理解解析逻辑

4. **开始开发**：
    - 在 `issues` 中寻找标记为 "good first issue" 的任务
    - 遵循项目编码规范
    - 提交 PR 前运行所有测试

5. **更新指令**:
    - 在 `.github/copilot-instructions.md`中同步更新最新的更改造成的影响。

通过遵循这些指南，开发者可以高效地为 TMXParser 项目做出贡献，同时确保代码质量和项目的长期可维护性。