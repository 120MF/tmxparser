# SDL3 Animation Example

这个示例展示了如何使用 TMXParser 解析和渲染带有动画的 Tiled 地图。

## 特性

- ✅ **外部 TSX 瓦片集**: 解析 `.tsx` 文件引用的瓦片集
- ✅ **瓦片动画**: 支持帧动画序列
- ✅ **时间驱动**: 基于实际时间更新动画帧
- ✅ **多动画支持**: 同时运行多个独立的动画
- ✅ **动画状态管理**: 高效跟踪每个动画的当前状态
- ✅ **混合渲染**: 动画瓦片与静态瓦片无缝混合

## 使用的地图文件

- **test_animation.tmx**: 包含动画瓦片的地图
- **fire_animation.tsx**: 外部瓦片集定义（包含33个火焰动画）
- **fire_animation.png**: 瓦片集图像（352x576px, 11列18行）

## 动画工作原理

### 1. 解析阶段

外部 TSX 文件被自动加载：

```xml
<tileset firstgid="5" source="fire_animation.tsx"/>
```

TSX 文件中定义的动画：

```xml
<tile id="0">
  <animation>
    <frame tileid="0" duration="150"/>
    <frame tileid="33" duration="150"/>
    <frame tileid="66" duration="150"/>
    ...
  </animation>
</tile>
```

### 2. 数据预处理

TMXParser 在解析时预处理动画数据：

```cpp
struct TileAnimationInfo {
    uint32_t baseTileId;                    // 基础瓦片ID
    std::vector<AnimationFrameInfo> frames; // 所有帧信息
    uint32_t totalDuration;                 // 总时长（毫秒）
};

struct AnimationFrameInfo {
    uint32_t tileId;    // 帧瓦片ID
    uint32_t srcX;      // 预计算的源X坐标
    uint32_t srcY;      // 预计算的源Y坐标
    uint32_t duration;  // 帧持续时间
};
```

### 3. 运行时渲染

动画状态跟踪：

```cpp
struct AnimationState {
    uint32_t currentFrame;  // 当前帧索引
    uint32_t elapsedTime;   // 已过时间
};

// 更新动画
state.elapsedTime += deltaTime;
uint32_t timeInCycle = state.elapsedTime % animation.totalDuration;

// 找到当前帧
uint32_t accumulatedTime = 0;
for (size_t i = 0; i < animation.frames.size(); ++i) {
    accumulatedTime += animation.frames[i].duration;
    if (timeInCycle < accumulatedTime) {
        state.currentFrame = i;
        break;
    }
}

// 使用当前帧渲染
const auto& frame = animation.frames[state.currentFrame];
SDL_FRect srcRect = { frame.srcX, frame.srcY, tile.srcW, tile.srcH };
```

## 性能优化

1. **预计算帧坐标**: 所有帧的源矩形坐标在解析时计算
2. **状态哈希映射**: 使用 `unordered_map` 快速查找动画状态
3. **循环时间**: 使用模运算实现循环播放，无需重置
4. **增量更新**: 只更新增量时间，不重新计算整个动画

## 构建和运行

```bash
# 从项目根目录构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TMX_EXAMPLES=ON
make -j$(nproc)

# 运行动画示例
./examples/SDL3/animated/tmxparser_sdl3_animated
```

## 扩展建议

1. **动画控制**:
   - 添加暂停/继续功能
   - 实现播放速度控制
   - 支持反向播放

2. **高级效果**:
   - 添加动画混合
   - 实现过渡效果
   - 支持帧事件回调

3. **性能增强**:
   - 实现动画对象池
   - 添加视锥裁剪
   - 使用 GPU 加速

## 代码结构

```
animated/
├── main.cpp              # 主程序
├── CMakeLists.txt        # 构建配置
└── README.md             # 本文档

使用的公共库:
../common/
├── sdl3_utils.hpp        # SDL3 工具函数声明
├── sdl3_utils.cpp        # SDL3 工具函数实现
└── CMakeLists.txt        # 公共库构建配置
```

## 许可

与 TMXParser 项目使用相同的许可证。
