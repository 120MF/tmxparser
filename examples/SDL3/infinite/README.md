# TMXParser SDL3 Infinite Map Example

This example demonstrates rendering of infinite TMX maps using SDL3. It renders the `Interior1.tmx` map from the `assets/infinite/` directory.

## Features

- **Infinite Map Support**: Parses and renders maps with chunk-based data
- **Camera Panning**: Use arrow keys to pan around the map
- **Animation Support**: Animated tiles (doors, windows) are rendered correctly
- **Auto-scaling**: The map is scaled to fit within a 1280x720 window if needed
- **Optimized Rendering**: Only non-empty tiles are rendered using pre-calculated positions

## Controls

- **Arrow Keys**: Pan the camera around the infinite map
- **ESC**: Quit the application

## Map Details

The `Interior1.tmx` example map features:
- 7 layers (Floor, Tile Layer 6, Boxes, Walls, Windows, Objects1, Objects2)
- 25 chunks total across all layers
- 1,084 renderable tiles (empty tiles are automatically skipped)
- 3 tilesets including animated door and window tiles
- Map bounds: approximately 416x352 pixels

## Building

The example is built automatically when you enable examples:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TMX_EXAMPLES=ON
make -j$(nproc)
```

## Running

```bash
./examples/SDL3/infinite/tmxparser_sdl3_infinite
```

## Implementation Notes

### Infinite Map Parsing

Infinite maps use `<chunk>` elements instead of continuous tile data:

```xml
<layer id="3" name="Floor" width="16" height="24">
  <data encoding="csv">
    <chunk x="-16" y="-16" width="16" height="16">
      <!-- CSV tile data for this chunk -->
    </chunk>
    <!-- More chunks... -->
  </data>
</layer>
```

Each chunk has:
- `x`, `y`: Position in tile coordinates (can be negative!)
- `width`, `height`: Size of the chunk in tiles
- Tile data: CSV-encoded tile IDs

### Rendering Approach

1. **Parse chunks**: The parser reads all chunks and their tile data
2. **Pre-calculate positions**: `RenderData.cpp` converts chunk-relative positions to absolute screen positions
3. **Calculate bounds**: Find the bounding box of all tiles to determine camera offset
4. **Render with offset**: Apply camera position and offset to each tile during rendering

### Camera System

The example includes a simple camera system:
- Calculates the bounding box of all rendered tiles
- Applies an offset to center the visible content
- Supports panning with arrow keys

This is useful for infinite maps where tiles can have negative coordinates.

## Performance

The rendering is highly optimized:
- ✅ All tile positions pre-calculated once during initialization
- ✅ No per-frame math calculations for tile positions
- ✅ Empty tiles are completely skipped
- ✅ Only 1,084 tiles rendered out of 6,400 possible chunk tiles
- ✅ Animations use pre-computed frame lookup tables

## See Also

- [Basic Example](../basic/) - Simple static map rendering
- [Animation Example](../animated/) - Tile animations with external tilesets
- [Object Example](../object/) - Rendering object layers
