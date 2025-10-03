# TMXParser

A modern C++23 library for parsing TMX (Tiled Map Exchange) format files used by the [Tiled Map Editor](https://www.mapeditor.org/).

## Features

- **Modern C++23** - Utilizes latest language features for type safety and performance
- **TMX Standard Support** - Fully compliant with [Tiled Map Editor specifications](https://doc.mapeditor.org/en/stable/reference/tmx-map-format/)
- **Pure Parsing Library** - Focused on TMX format parsing; rendering handled at application level
- **Render-Agnostic Data Structures** - Provides platform-independent pre-computed rendering data
- **Multiple Encoding Support** - CSV, Base64, Base64+gzip, Base64+zlib, Base64+zstd
- **Error Handling** - Modern error handling using `tl::expected`
- **Easy Integration** - CMake package support for simple integration into projects

## Quick Start

### Installation

```bash
# Clone and build
git clone https://github.com/120MF/tmxparser.git
cd tmxparser
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Install to system or custom prefix
cmake --install . --prefix /usr/local
```

For detailed installation instructions, see [INSTALL.md](INSTALL.md).

### Using in Your Project

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(my_game)

set(CMAKE_CXX_STANDARD 23)

# Find and link tmxparser
find_package(tmxparser REQUIRED CONFIG)
add_executable(my_game main.cpp)
target_link_libraries(my_game PRIVATE tmxparser::tmxparser)
```

**main.cpp:**
```cpp
#include <tmx/tmx.hpp>
#include <iostream>

int main() {
    // Parse TMX file
    auto result = tmx::Parser::parseFromFile("map.tmx");
    if (!result) {
        std::cerr << "Parse error: " << result.error() << std::endl;
        return 1;
    }
    
    const auto& map = *result;
    std::cout << "Map: " << map.width << "x" << map.height << std::endl;
    
    // Create pre-computed render data
    auto renderData = tmx::render::createRenderData(map);
    // Use renderData for efficient rendering
    
    return 0;
}
```

## Supported TMX Features

- ✅ Basic map properties (size, tile size, orientation)
- ✅ Tileset management (firstgid, image paths, tile dimensions)
- ✅ Layer data (CSV and Base64 encoding)
- ✅ Complete compression support (zlib, gzip, zstd)
- ✅ Properties system (key-value pairs)
- ✅ Color parsing (hexadecimal format)
- ✅ Tile animations
- ✅ External tilesets (.tsx files)
- ✅ Object layers (points, rectangles, ellipses, polygons, polylines)
- ✅ Infinite maps (chunk-based rendering)
- ⭕ Image layers (planned)
- ⭕ Group layers (planned)

## Architecture

### Core Components

```
include/tmx/
├── tmx.hpp         # Main header - includes everything
├── Map.hpp         # TMX data structures
├── Parser.hpp      # Parsing interface
└── RenderData.hpp  # Pre-computed rendering structures
```

### Data Flow

```
TMX File → Parser → Map Structure → RenderData → Your Renderer
```

The library provides parsed TMX data and optionally pre-computed rendering information. Actual rendering is implemented in your application using your preferred graphics API (SDL, OpenGL, Vulkan, etc.).

## Examples

The repository includes examples demonstrating library usage:

### Basic Example
```bash
cmake .. -DBUILD_TMX_EXAMPLES=ON
make
./examples/basic/tmxparser_basic_example
```

### SDL3 Rendering Examples
Complete rendering implementations using SDL3:
```bash
# Basic rendering
./examples/SDL3/basic/tmxparser_sdl3_basic

# Animated tiles
./examples/SDL3/animated/tmxparser_sdl3_animated

# Object layer rendering
./examples/SDL3/object/tmxparser_sdl3_object

# Infinite map rendering (with camera panning)
./examples/SDL3/infinite/tmxparser_sdl3_infinite
```

See [examples/SDL3/](examples/SDL3/) directory for full source code and documentation.

## Testing

The library includes comprehensive unit tests:

```bash
cmake .. -DBUILD_TMX_TESTS=ON -DCMAKE_BUILD_TYPE=Release
make
ctest --output-on-failure
```

Tests cover all supported encoding and compression formats.

## Dependencies

All dependencies are automatically fetched and built via CMake FetchContent:

- [pugixml](https://github.com/zeux/pugixml) - XML parsing
- [tl::expected](https://github.com/TartanLlama/expected) - Error handling
- [base64](https://github.com/aklomp/base64) - Base64 decoding
- [zlib](https://github.com/madler/zlib) - Compression
- [zstd](https://github.com/facebook/zstd) - High-efficiency compression

## Requirements

- **CMake**: 3.20 or higher
- **Compiler**: C++23 support required
  - GCC 13+
  - Clang 16+
  - MSVC 2022+

## Platform Support

- ✅ Linux (primary development platform)
- 🔄 Windows (planned)
- 🔄 macOS (planned)

## Performance

The library is designed with performance in mind:

- **Pre-computed rendering data** - Eliminates runtime calculations
- **Cache-friendly memory layout** - Optimized for modern CPUs
- **Sparse tile storage** - Only non-empty tiles stored in render data
- **Zero-copy where possible** - Efficient memory usage

## Contributing

Contributions are welcome! Please check the issues page for tasks labeled "good first issue".

### Development Guidelines

- Follow C++23 best practices
- Use `tl::expected` for error handling
- Write unit tests for new features
- Update documentation

## License

[License information to be added]

## Acknowledgments

- [Tiled Map Editor](https://www.mapeditor.org/) - For the excellent map editor and TMX format
- All dependency library authors

## See Also

- [INSTALL.md](INSTALL.md) - Detailed installation and integration guide
- [TMX Format Documentation](https://doc.mapeditor.org/en/stable/reference/tmx-map-format/)
- [Tiled Map Editor](https://www.mapeditor.org/)
