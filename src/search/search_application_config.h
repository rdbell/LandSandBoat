#pragma once

#include "common/cbasetypes.h"

#include <array>
#include <string>

struct SearchConsoleCommandDescriptor
{
    std::string name;
    std::string description;
};

auto SearchApplicationServerName() -> std::string;
auto SearchApplicationConsoleCommandDescriptors(uint16 expiryDays) -> std::array<SearchConsoleCommandDescriptor, 2>;
