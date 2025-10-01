#include "tmx/Map.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace tmx::map
{
    auto Color::fromString(const std::string& hex) -> tl::expected<Color, std::string>
    {
        if (hex.empty())
        {
            return Color{255, 255, 255, 255}; // Default white
        }

        std::string hexStr = hex;
        if (hexStr[0] == '#')
        {
            hexStr = hexStr.substr(1);
        }

        if (hexStr.length() != 6 && hexStr.length() != 8)
        {
            return tl::make_unexpected("Invalid hex color format: " + hex);
        }

        try
        {
            const unsigned long value = std::stoul(hexStr, nullptr, 16);

            if (hexStr.length() == 6)
            {
                // RGB format
                return Color{
                    static_cast<std::uint8_t>((value >> 16) & 0xFF),
                    static_cast<std::uint8_t>((value >> 8) & 0xFF),
                    static_cast<std::uint8_t>(value & 0xFF),
                    255
                };
            }
            else
            {
                // RGBA format
                return Color{
                    static_cast<std::uint8_t>((value >> 24) & 0xFF),
                    static_cast<std::uint8_t>((value >> 16) & 0xFF),
                    static_cast<std::uint8_t>((value >> 8) & 0xFF),
                    static_cast<std::uint8_t>(value & 0xFF)
                };
            }
        }
        catch (const std::exception& e)
        {
            return tl::make_unexpected("Failed to parse hex color: " + std::string(e.what()));
        }
    }

    auto Properties::get(const std::string& name) const -> std::string
    {
        const auto it = std::ranges::find_if(properties,
                                             [&name](const Property& prop) { return prop.name == name; });
        return it != properties.end() ? it->value : "";
    }

    auto Properties::getInt(const std::string& name, const int defaultValue) const -> int
    {
        const auto value = get(name);
        if (value.empty()) return defaultValue;

        try
        {
            return std::stoi(value);
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    auto Properties::getFloat(const std::string& name, const float defaultValue) const -> float
    {
        const auto value = get(name);
        if (value.empty()) return defaultValue;

        try
        {
            return std::stof(value);
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    auto Properties::getBool(const std::string& name, const bool defaultValue) const -> bool
    {
        const auto value = get(name);
        if (value.empty()) return defaultValue;

        return value == "true" || value == "1";
    }
}
