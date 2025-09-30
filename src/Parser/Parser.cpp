#include "tmx/Parser/Parser.hpp"
#include <fstream>
#include <sstream>
#include <zlib.h>
#include <libbase64.h>

namespace tmx
{
    auto Parser::parseFromFile(const std::filesystem::path& path) -> Result
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            return tl::make_unexpected("Cannot open file: " + path.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return parseFromString(buffer.str());
    }

    auto Parser::parseFromString(const std::string& xml) -> Result
    {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_string(xml.c_str());

        if (!result)
        {
            return tl::make_unexpected("XML parsing error: " + std::string(result.description()));
        }

        const auto mapNode = doc.child("map");
        if (!mapNode)
        {
            return tl::make_unexpected("No 'map' element found in XML");
        }

        return parseMap(mapNode);
    }

    auto Parser::parseMap(const pugi::xml_node& mapNode) -> Result
    {
        Map map;

        // Parse attributes
        map.version = mapNode.attribute("version").as_string("1.0");
        map.tiledversion = mapNode.attribute("tiledversion").as_string();
        map.orientation = parseOrientation(mapNode.attribute("orientation").as_string("orthogonal"));
        map.renderorder = parseRenderOrder(mapNode.attribute("renderorder").as_string("right-down"));
        map.width = mapNode.attribute("width").as_uint();
        map.height = mapNode.attribute("height").as_uint();
        map.tilewidth = mapNode.attribute("tilewidth").as_uint();
        map.tileheight = mapNode.attribute("tileheight").as_uint();
        map.infinite = mapNode.attribute("infinite").as_bool();
        map.nextlayerid = mapNode.attribute("nextlayerid").as_uint(1);
        map.nextobjectid = mapNode.attribute("nextobjectid").as_uint(1);

        // Parse background color
        if (auto bgColorAttr = mapNode.attribute("backgroundcolor"))
        {
            auto colorResult = Color::fromString(bgColorAttr.as_string());
            if (colorResult)
            {
                map.backgroundcolor = *colorResult;
            }
        }

        // Parse properties
        if (auto propertiesNode = mapNode.child("properties"))
        {
            map.properties = parseProperties(propertiesNode);
        }

        // Parse tilesets
        for (auto tilesetNode : mapNode.children("tileset"))
        {
            auto tilesetResult = parseTileset(tilesetNode);
            if (!tilesetResult)
            {
                return tl::make_unexpected(tilesetResult.error());
            }
            map.tilesets.push_back(*tilesetResult);
        }

        // Parse layers
        for (auto layerNode : mapNode.children("layer"))
        {
            auto layerResult = parseLayer(layerNode);
            if (!layerResult)
            {
                return tl::make_unexpected(layerResult.error());
            }
            map.layers.push_back(*layerResult);
        }

        return map;
    }

    auto Parser::parseTileset(const pugi::xml_node& tilesetNode) -> tl::expected<Tileset, std::string>
    {
        Tileset tileset{};

        tileset.firstgid = tilesetNode.attribute("firstgid").as_uint();
        tileset.name = tilesetNode.attribute("name").as_string();
        tileset.tilewidth = tilesetNode.attribute("tilewidth").as_uint();
        tileset.tileheight = tilesetNode.attribute("tileheight").as_uint();
        tileset.tilecount = tilesetNode.attribute("tilecount").as_uint();
        tileset.columns = tilesetNode.attribute("columns").as_uint();
        tileset.source = tilesetNode.attribute("source").as_string();

        // Parse image
        if (const auto imageNode = tilesetNode.child("image"))
        {
            tileset.image = imageNode.attribute("source").as_string();
            tileset.imagewidth = imageNode.attribute("width").as_uint();
            tileset.imageheight = imageNode.attribute("height").as_uint();
        }

        // Parse properties
        if (const auto propertiesNode = tilesetNode.child("properties"))
        {
            tileset.properties = parseProperties(propertiesNode);
        }

        return tileset;
    }

    auto Parser::parseLayer(const pugi::xml_node& layerNode) -> tl::expected<Layer, std::string>
    {
        Layer layer;

        layer.name = layerNode.attribute("name").as_string();
        layer.width = layerNode.attribute("width").as_uint();
        layer.height = layerNode.attribute("height").as_uint();
        layer.visible = layerNode.attribute("visible").as_bool(true);
        layer.opacity = layerNode.attribute("opacity").as_float(1.0f);

        // Parse properties
        if (const auto propertiesNode = layerNode.child("properties"))
        {
            layer.properties = parseProperties(propertiesNode);
        }

        // Parse data
        if (const auto dataNode = layerNode.child("data"))
        {
            auto dataResult = parseData(dataNode, layer.width, layer.height);
            if (!dataResult)
            {
                return tl::make_unexpected(dataResult.error());
            }
            layer.data = *dataResult;
        }

        return layer;
    }

    auto Parser::parseProperties(const pugi::xml_node& propertiesNode) -> Properties
    {
        Properties properties;

        for (auto propertyNode : propertiesNode.children("property"))
        {
            Property prop;
            prop.name = propertyNode.attribute("name").as_string();
            prop.value = propertyNode.attribute("value").as_string();
            prop.type = propertyNode.attribute("type").as_string("string");

            properties.properties.push_back(prop);
        }

        return properties;
    }

    auto Parser::parseOrientation(const std::string& str) -> Orientation
    {
        if (str == "isometric") return Orientation::Isometric;
        if (str == "staggered") return Orientation::Staggered;
        if (str == "hexagonal") return Orientation::Hexagonal;
        return Orientation::Orthogonal;
    }

    auto Parser::parseRenderOrder(const std::string& str) -> RenderOrder
    {
        if (str == "right-up") return RenderOrder::RightUp;
        if (str == "left-down") return RenderOrder::LeftDown;
        if (str == "left-up") return RenderOrder::LeftUp;
        return RenderOrder::RightDown;
    }

    auto Parser::parseData(const pugi::xml_node& dataNode, std::uint32_t width, std::uint32_t height)
        -> tl::expected<std::vector<std::uint32_t>, std::string>
    {
        std::vector<std::uint32_t> data;
        std::string encoding = dataNode.attribute("encoding").as_string();
        std::string compression = dataNode.attribute("compression").as_string();

        if (encoding == "csv")
        {
            // Parse CSV data
            std::string csvData = dataNode.text().as_string();
            std::stringstream ss(csvData);
            std::string cell;

            while (std::getline(ss, cell, ','))
            {
                try
                {
                    data.push_back(static_cast<std::uint32_t>(std::stoul(cell)));
                }
                catch (...)
                {
                    return tl::make_unexpected("Failed to parse CSV data");
                }
            }
        }
        else if (encoding == "base64")
        {
            // Parse base64 encoded data
            std::string base64Data = dataNode.text().as_string();

            // Remove whitespace
            base64Data.erase(std::remove_if(base64Data.begin(), base64Data.end(), ::isspace),
                             base64Data.end());

            // Decode base64
            size_t outLen;
            std::vector<char> decoded(base64Data.length()); // Pre-allocate buffer

            if (base64_decode(base64Data.c_str(), base64Data.length(), decoded.data(), &outLen, 0) == 0)
            {
                return tl::make_unexpected("Failed to decode base64 data");
            }

            // Handle compression
            std::vector<char> decompressed;
            if (compression == "zlib")
            {
                // Decompress with zlib
                z_stream stream;
                stream.zalloc = Z_NULL;
                stream.zfree = Z_NULL;
                stream.opaque = Z_NULL;
                stream.avail_in = static_cast<uInt>(outLen);
                stream.next_in = reinterpret_cast<Bytef*>(decoded.data());

                if (inflateInit(&stream) != Z_OK)
                {
                    return tl::make_unexpected("Failed to initialize zlib decompression");
                }

                decompressed.resize(width * height * 4); // 4 bytes per tile ID
                stream.avail_out = static_cast<uInt>(decompressed.size());
                stream.next_out = reinterpret_cast<Bytef*>(decompressed.data());

                int result = inflate(&stream, Z_FINISH);
                inflateEnd(&stream);

                if (result != Z_STREAM_END)
                {
                    return tl::make_unexpected("Failed to decompress zlib data");
                }
            }
            else
            {
                // No compression
                decompressed.assign(decoded.begin(), decoded.begin() + outLen);
            }

            // Convert bytes to uint32_t values
            for (size_t i = 0; i < decompressed.size(); i += 4)
            {
                if (i + 3 < decompressed.size())
                {
                    std::uint32_t tileId =
                        static_cast<std::uint8_t>(decompressed[i]) |
                        (static_cast<std::uint8_t>(decompressed[i + 1]) << 8) |
                        (static_cast<std::uint8_t>(decompressed[i + 2]) << 16) |
                        (static_cast<std::uint8_t>(decompressed[i + 3]) << 24);
                    data.push_back(tileId);
                }
            }
        }
        else
        {
            return tl::make_unexpected("Unsupported encoding: " + encoding);
        }

        return data;
    }
}
