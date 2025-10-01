# SDL3 渲染示例

这是一个完整的 SDL3 渲染示例，展示了如何使用 TMXParser 库解析和渲染 Tiled 地图编辑器导出的 TMX 文件。

## 功能特性

- ✅ 使用 TMXParser 解析 TMX 文件
- ✅ 使用 SDL3 创建窗口和渲染器
- ✅ 使用 stb_image 加载 PNG 格式的瓦片集图像
- ✅ 正确计算瓦片 ID 和渲染位置
- ✅ 支持多图层渲染（当前示例渲染第一层）
- ✅ 事件处理（ESC 键退出、窗口关闭）
- ✅ 60 FPS 渲染循环

## 构建和运行

### 前提条件

在 Linux 系统上，需要安装 X11 开发库：

```bash
# Ubuntu/Debian
sudo apt-get install libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxinerama-dev libxxf86vm-dev

# Fedora/RHEL
sudo dnf install libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXfixes-devel libXi-devel libXinerama-devel libXxf86vm-devel
```

### 构建

```bash
# 从项目根目录
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 运行

```bash
# 从 build 目录
./examples/SDL3/tmxparser_sdl3_example
```

示例会加载 `assets/test.tmx` 地图文件和 `assets/test_tileset.png` 瓦片集图像，并在窗口中渲染地图。

## 代码结构

### 主要步骤

1. **解析 TMX 文件**
   ```cpp
   auto result = tmx::Parser::parseFromFile(assetDir / "test.tmx");
   ```

2. **初始化 SDL3**
   ```cpp
   SDL_Init(SDL_INIT_VIDEO);
   SDL_Window* window = SDL_CreateWindow(
       "TMXParser SDL3 Example",
       windowWidth, windowHeight, 0
   );
   SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
   ```

3. **加载瓦片集纹理**
   ```cpp
   // 使用 stb_image 加载 PNG
   unsigned char* imageData = stbi_load(imageSource.string().c_str(), &width, &height, &channels, 4);
   
   // 创建 SDL 表面和纹理
   SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, imageData, width * 4);
   SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
   ```

4. **渲染循环**
   ```cpp
   while (running) {
       // 处理事件
       while (SDL_PollEvent(&event)) { ... }
       
       // 清屏
       SDL_RenderClear(renderer);
       
       // 渲染瓦片
       for (uint32_t y = 0; y < layer.height; ++y) {
           for (uint32_t x = 0; x < layer.width; ++x) {
               // 计算源和目标矩形
               // 渲染瓦片
               SDL_RenderTexture(renderer, texture, &srcRect, &destRect);
           }
       }
       
       // 呈现
       SDL_RenderPresent(renderer);
   }
   ```

### 瓦片 ID 计算

TMX 文件中的 GID（Global ID）需要减去瓦片集的 firstgid 才能得到正确的瓦片 ID：

```cpp
uint32_t gid = layer.data[y * layer.width + x];
uint32_t tileId = gid - tileset.firstgid;
```

然后根据瓦片 ID 和瓦片集的列数计算在瓦片集图像中的位置：

```cpp
uint32_t tileX = (tileId % tileset.columns) * tileset.tilewidth;
uint32_t tileY = (tileId / tileset.columns) * tileset.tileheight;
```

## 依赖项

- **TMXParser**: 核心解析库
- **SDL3**: 图形渲染库（自动通过 FetchContent 下载）
- **stb_image**: 图像加载库（自动通过 FetchContent 下载）

## 控制

- **ESC 键**: 退出程序
- **窗口关闭按钮**: 退出程序

## 扩展

你可以基于这个示例进行扩展：

1. **支持多图层**: 遍历所有图层并渲染
2. **支持图层透明度**: 使用 `layer.opacity` 设置渲染透明度
3. **支持图层可见性**: 检查 `layer.visible` 决定是否渲染
4. **添加相机控制**: 实现地图滚动和缩放
5. **性能优化**: 
   - 实现视锥裁剪，只渲染可见区域
   - 使用批量渲染减少绘制调用
   - 缓存瓦片纹理

## 故障排除

### "Failed to initialize SDL3: No available video device"

这个错误通常出现在无头（headless）环境中，例如 CI/CD 环境或没有 X11 服务器的 SSH 会话。

解决方案：
- 在本地机器上运行（有图形界面）
- 使用虚拟显示器（Xvfb）
- 使用 SDL 的软件渲染器

### "Failed to load tileset image"

确保：
- `assets/test_tileset.png` 文件存在
- 文件路径正确（相对于可执行文件）
- 图像文件格式正确（PNG）

## 许可

与 TMXParser 项目使用相同的许可证。
