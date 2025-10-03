# SDL3 渲染示例

这是完整的 SDL3 渲染示例集合，展示了如何使用 TMXParser 库解析和渲染 Tiled 地图编辑器导出的 TMX 文件。

## 示例列表

### 1. Basic (基础渲染)

位于 `basic/` 目录，展示基本的静态地图渲染。

**特性：**
- ✅ 使用 TMXParser 解析 TMX 文件
- ✅ 使用 SDL3 创建窗口和渲染器
- ✅ 使用 stb_image 加载 PNG 格式的瓦片集图像
- ✅ 预计算渲染数据，无运行时计算
- ✅ 支持多图层渲染和透明度
- ✅ 事件处理（ESC 键退出、窗口关闭）
- ✅ 60 FPS 渲染循环

### 2. Animated (动画渲染)

位于 `animated/` 目录，展示外部 TSX 瓦片集和瓦片动画的渲染。

**特性：**
- ✅ 外部 TSX 瓦片集解析 (.tsx 文件)
- ✅ 瓦片动画系统
- ✅ 基于时间的帧更新
- ✅ 多个动画同时运行
- ✅ 动画状态管理
- ✅ 与静态瓦片混合渲染

### 3. Object (对象渲染)

位于 `object/` 目录，展示对象层的解析和渲染。

**特性：**
- ✅ 对象层解析
- ✅ 支持多种对象形状（点、矩形、椭圆、多边形、折线）
- ✅ 基于对象类型的颜色编码
- ✅ 瓦片层与对象层混合渲染
- ✅ 动画瓦片背景支持

### 4. Common (公共库)

位于 `common/` 目录，提供所有示例共享的工具函数。

**功能：**
- SDL3 初始化和窗口创建
- 瓦片集纹理加载
- 动画状态管理
- 地图和图层渲染函数
- 资源清理
- 错误处理

## 构建和运行

### 前提条件

在 Linux 系统上，需要安装 X11 开发库：

```bash
# Ubuntu/Debian
sudo apt-get install libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxinerama-dev

# Fedora/RHEL
sudo dnf install libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXfixes-devel libXi-devel libXinerama-devel
```

### 构建

```bash
# 从项目根目录
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TMX_EXAMPLES=ON
make -j$(nproc)
```

### 运行

```bash
# 基础示例
./examples/SDL3/basic/tmxparser_sdl3_basic

# 动画示例
./examples/SDL3/animated/tmxparser_sdl3_animated

# 对象示例
./examples/SDL3/object/tmxparser_sdl3_object
```

## 架构设计

### 公共库 (tmxparser_sdl3_common)

提供可重用的 SDL3 工具函数，避免代码重复。

### 渲染数据预计算

TMXParser 提供 `MapRenderData` 结构，预计算所有瓦片的渲染信息，消除运行时计算开销。

### 动画系统

动画数据在解析时提取并预处理，运行时只需跟踪时间状态和当前帧。

## 依赖项

- **TMXParser**: 核心解析库
- **SDL3**: 图形渲染库（自动通过 FetchContent 下载）
- **stb_image**: 图像加载库（自动通过 FetchContent 下载）

## 控制

所有示例：
- **ESC 键**: 退出程序
- **窗口关闭按钮**: 退出程序

## 性能特点

1. **预计算优势**：
   - 无需每帧计算瓦片位置
   - 仅遍历非空瓦片
   - 缓存友好的数据结构

2. **渲染效率**：
   - 直接使用预计算的矩形
   - 最小化状态切换
   - 批量渲染同一瓦片集

## 许可

与 TMXParser 项目使用相同的许可证。
