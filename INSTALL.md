# TMXParser Installation Guide

This document explains how to build, install, and use the TMXParser library in your CMake projects.

## Building and Installing

### Prerequisites

- CMake 3.20 or higher
- C++23 compatible compiler (GCC 13+, Clang 16+, or MSVC 2022+)
- Git (for fetching dependencies)

### Build Steps

```bash
# Clone the repository
git clone https://github.com/120MF/tmxparser.git
cd tmxparser

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
make -j$(nproc)

# Install to a custom prefix (e.g., /usr/local or ~/.local)
cmake --install . --prefix /path/to/install
```

### Installation Directories

The installation will create the following structure:

```
<prefix>/
├── include/
│   └── tmx/
│       ├── Map.hpp
│       ├── Parser.hpp
│       ├── RenderData.hpp
│       └── tmx.hpp
├── lib/
│   ├── libtmxparser.a
│   └── cmake/
│       └── tmxparser/
│           ├── tmxparserConfig.cmake
│           ├── tmxparserConfigVersion.cmake
│           ├── tmxparserTargets.cmake
│           └── Modules/
│               └── (dependency finding modules)
```

## Using TMXParser in Your CMake Project

### CMakeLists.txt Configuration

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_project)

set(CMAKE_CXX_STANDARD 23)

# Find the installed tmxparser package
find_package(tmxparser REQUIRED CONFIG)

# Create your executable
add_executable(my_app main.cpp)

# Link against tmxparser using the namespace
target_link_libraries(my_app PRIVATE tmxparser::tmxparser)
```

### Specifying Installation Prefix

If you installed tmxparser to a custom location, you need to tell CMake where to find it:

```bash
# Option 1: Set CMAKE_PREFIX_PATH
cmake -DCMAKE_PREFIX_PATH=/path/to/install ..

# Option 2: Set tmxparser_DIR directly
cmake -Dtmxparser_DIR=/path/to/install/lib/cmake/tmxparser ..
```

### Example Code

```cpp
#include <tmx/tmx.hpp>
#include <iostream>

int main() {
    // Parse a TMX file
    auto result = tmx::Parser::parseFromFile("map.tmx");
    
    if (!result) {
        std::cerr << "Failed to parse: " << result.error() << std::endl;
        return 1;
    }
    
    const auto& map = *result;
    std::cout << "Map size: " << map.width << "x" << map.height << std::endl;
    std::cout << "Tile size: " << map.tilewidth << "x" << map.tileheight << std::endl;
    
    // Create render data (pre-computed rendering information)
    auto renderData = tmx::render::createRenderData(map);
    std::cout << "Layers: " << renderData.layers.size() << std::endl;
    
    return 0;
}
```

## System-Wide Installation (Linux)

For system-wide installation on Linux:

```bash
# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local

# Install (requires root privileges)
sudo make install
```

After system-wide installation, you can use tmxparser in any project without specifying `CMAKE_PREFIX_PATH`.

## Dependencies

TMXParser automatically handles its dependencies during the build process using CMake's FetchContent:

- **pugixml** - XML parsing
- **tl::expected** - Error handling
- **base64** - Base64 decoding
- **zlib** - Data decompression
- **zstd** - High-efficiency compression

All dependencies are installed alongside tmxparser and are automatically located when you use `find_package(tmxparser)`.

## Troubleshooting

### CMake can't find tmxparser

Make sure you've specified the correct installation prefix:

```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/install ..
```

### Linking errors

Ensure you're using C++23 standard in your project:

```cmake
set(CMAKE_CXX_STANDARD 23)
```

### Missing dependencies

All dependencies should be installed automatically with tmxparser. If you encounter missing dependency errors, ensure the installation completed successfully.

## Advanced Options

### Building Examples

```bash
cmake .. -DBUILD_TMX_EXAMPLES=ON
```

### Building Tests

```bash
cmake .. -DBUILD_TMX_TESTS=ON
```

### Static vs Shared Library

Currently, tmxparser is built as a static library by default. To use it as a shared library, modify `src/CMakeLists.txt` to change `STATIC` to `SHARED`.

## Version Compatibility

The installed package includes version information. You can specify version requirements in your CMakeLists.txt:

```cmake
find_package(tmxparser 1.0 REQUIRED CONFIG)
```

## Uninstallation

CMake doesn't provide a built-in uninstall target, but you can manually remove the installed files:

```bash
# Remove headers
rm -rf /path/to/install/include/tmx

# Remove library
rm /path/to/install/lib/libtmxparser.a

# Remove CMake files
rm -rf /path/to/install/lib/cmake/tmxparser
```
