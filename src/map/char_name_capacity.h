#pragma once

#include "common/utils.h"

#include <string>

namespace charnamehelpers
{

inline auto Normalize(std::string name) -> std::string
{
    if (name.size() > PacketNameLength)
    {
        name.resize(PacketNameLength);
    }
    return name;
}

} // namespace charnamehelpers
