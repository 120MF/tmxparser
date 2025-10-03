# SDL3 Object Example

This example demonstrates how to parse and render TMX maps with object layers using TMXParser and SDL3.

## Features

- ✅ Parse object layers from TMX files
- ✅ Support for different object shapes:
  - Point objects (rendered as crosshairs)
  - Rectangle objects
  - Ellipse objects
  - Polygon objects
  - Polyline objects
- ✅ Color-coded rendering based on object type
- ✅ Render both tile layers and objects
- ✅ Support for animated tiles in the background

## Map File

The example uses `assets/object/island.tmx`, which contains:
- Multiple tile layers (Ground, Fringe, Over)
- An object layer with various object types:
  - Starting Point (green point)
  - Exit (red rectangle)
  - Resting Spot (blue rectangle)

## How It Works

1. **Parse TMX file**: Load and parse the TMX map with objects
2. **Create render data**: Pre-calculate tile positions and object information
3. **Render layers**: Use the common SDL3 rendering function for tile layers
4. **Render objects**: Draw objects with different colors based on their type

## Object Rendering

Objects are rendered with different shapes and colors:

- **Point**: Green crosshair (+) for "start" objects
- **Rectangle**: Red outline for "exit" objects, blue for "rest" objects
- **Ellipse**: Yellow outline drawn with line segments
- **Polygon**: Closed shape drawn with connected line segments
- **Polyline**: Open shape drawn with connected line segments

## Building

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TMX_EXAMPLES=ON
make tmxparser_sdl3_object
```

## Running

```bash
./examples/SDL3/object/tmxparser_sdl3_object
```

## Controls

- **ESC**: Exit the program
- **Window close button**: Exit the program

## Code Structure

The example demonstrates:
- Parsing TMX files with object layers
- Accessing object properties (position, size, type, shape)
- Rendering different object shapes with SDL3
- Combining tile layer rendering with object rendering

## Implementation Details

### Object Data

Each object contains:
- Position (x, y)
- Size (width, height) for rectangles and ellipses
- Type and name (custom properties)
- Shape type (point, rectangle, ellipse, polygon, polyline)
- Points array for polygon/polyline shapes

### Rendering Strategy

1. Render all tile layers first (using common rendering function)
2. Iterate through object groups
3. For each visible object, render based on its shape type
4. Use different colors to distinguish object types

## Extending the Example

Possible enhancements:

1. **Text Rendering**: Add SDL_ttf to render object names as actual text
2. **Interactive Objects**: Detect mouse clicks on objects
3. **Object Properties**: Display custom properties in a UI
4. **Editor-like Features**: Highlight selected objects, drag to move
5. **Tile Objects**: Render tile-based objects (gid != 0) with their sprite

## License

Uses the same license as the TMXParser project.
