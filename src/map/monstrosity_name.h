#pragma once

#include "common/cbasetypes.h"

namespace monstrosity
{

// PackName mirrors the little-endian value consumed by the Monstrosity client
// display: visible species in bits 0..15 and two name prefixes in bits 16..31.
constexpr auto PackName(const uint16 species, const uint8 prefix1, const uint8 prefix2) -> uint32
{
    return (static_cast<uint32>(prefix2) << 24) |
           (static_cast<uint32>(prefix1) << 16) |
           static_cast<uint32>(0x8000 | species);
}

} // namespace monstrosity
