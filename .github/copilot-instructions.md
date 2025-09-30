# TMXParser 项目 - GitHub Copilot 开发指南

## 项目概述

TMXParser 是一个现代化的 C++ TMX（Tiled Map Exchange）地图文件解析库，专门为游戏开发而设计。该项目采用最新的 C++23 标准，整合 SDL3 渲染引擎，提供高性能、缓存友好的地图解析和渲染功能。

### 核心特性
- **现代 C++23** - 利用最新语言特性提供类型安全和性能优化
- **TMX 标准支持** - 完全遵循 [Tiled Map Editor 官方规范](https://doc.mapeditor.org/en/stable/reference/tmx-map-format/)
- **SDL3 集成** - 为现代图形渲染提供无缝集成
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
│   └── Render.hpp       # 渲染器接口
├── src/                 # 源文件实现
│   ├── Map.cpp
│   ├── Parser.cpp
│   └── Render.cpp
├── examples/            # 示例代码
│   └── basic/          # 基础使用示例
├── tests/              # 单元测试
├── assets/             # 测试资源文件
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

#### 3. 渲染器 (`include/tmx/Render.hpp`)
```cpp
namespace tmx {
class Renderer {
public:
    auto loadMap(const map::Map& map) -> tl::expected<void, Error>;
    auto render(int x = 0, int y = 0) -> tl::expected<void, Error>;
};
}
```

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
- ✅ zlib/gzip 压缩支持
- ✅ 属性系统（键值对）
- ✅ 颜色解析（十六进制格式）

### 待实现功能
- ⭕ 对象层 (Object Layers)
- ⭕ 图像层 (Image Layers)
- ⭕ 组图层 (Group Layers)
- ⭕ 瓦片动画 (Tile Animations)
- ⭕ 地形类型 (Terrain Types)
- ⭕ 外部瓦片集 (.tsx 文件)
- ⭕ 无限地图 (Infinite Maps)
- ⭕ 六边形和等距地图支持

## SDL3 渲染集成

### 当前状态
目前渲染器提供基础接口，SDL3 集成处于初期阶段：

```cpp
// 基础用法
tmx::Renderer renderer(sdl_renderer);
auto result = renderer.loadMap(map);
if (result) {
    renderer.render(0, 0);  // x, y 偏移
}
```

### 渲染优化计划
1. **纹理管理**：
   - 瓦片集纹理缓存
   - 自动纹理图集生成
   - 动态纹理流送

2. **批量渲染**：
   - 实例化渲染
   - 绘制调用合并
   - GPU 缓冲区优化

3. **视锥裁剪**：
   - 只渲染可见瓦片
   - LOD (Level of Detail) 系统
   - 分块加载策略

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
- SDL3         # 渲染引擎
```

### 构建步骤
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 平台支持
- **Linux**: 主要开发平台，完全支持
- **Windows**: 计划支持，需要 Visual Studio 2022
- **macOS**: 计划支持，需要 Xcode 15+

## 测试策略

### 单元测试
- 解析器正确性测试
- 性能基准测试
- 内存泄漏检测
- 边界条件验证

### 集成测试
- 与实际游戏引擎集成测试
- 大型地图性能测试
- 多线程安全性验证

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

### 渲染集成
```cpp
#include <tmx/tmx.hpp>
#include <SDL3/SDL.h>

int main() {
    // SDL 初始化...
    SDL_Renderer* renderer = /* ... */;
    
    auto map_result = tmx::Parser::parseFromFile("level1.tmx");
    if (map_result) {
        tmx::Renderer tmx_renderer(renderer);
        tmx_renderer.loadMap(*map_result);
        
        // 渲染循环
        while (running) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            tmx_renderer.render(camera_x, camera_y);
            
            SDL_RenderPresent(renderer);
        }
    }
    
    return 0;
}
```

## 未来发展方向

### 短期目标 (1-2 个月)
1. 完善基础 TMX 功能支持
2. 实现高性能 SDL3 渲染
3. 添加完整的单元测试套件
4. 性能优化和基准测试

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
1. SDL3 集成需要完善图像加载
2. 错误处理需要更细粒度的错误类型
3. 内存池分配器待实现
4. 日志系统缺失

### 重构计划
1. 抽象渲染接口以支持多后端
2. 插件化架构设计
3. 配置系统标准化
4. 资源管理生命周期优化

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

通过遵循这些指南，开发者可以高效地为 TMXParser 项目做出贡献，同时确保代码质量和项目的长期可维护性。