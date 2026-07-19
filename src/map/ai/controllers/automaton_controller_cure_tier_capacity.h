#pragma once

#include <cstdint>

namespace automatoncontrollercuretier
{
enum class Tier : uint8_t
{
    I,
    II,
    III,
    IV,
    V,
    VI,
};

inline auto Select(int32_t missingHP) -> Tier
{
    if (missingHP > 850)
    {
        return Tier::VI;
    }
    if (missingHP > 600)
    {
        return Tier::V;
    }
    if (missingHP > 350)
    {
        return Tier::IV;
    }
    if (missingHP > 190)
    {
        return Tier::III;
    }
    if (missingHP > 120)
    {
        return Tier::II;
    }
    return Tier::I;
}
} // namespace automatoncontrollercuretier
