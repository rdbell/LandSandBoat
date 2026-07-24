#pragma once

#include "common/application.h"
#include "common/cbasetypes.h"

#include <array>
#include <string>

struct SearchConsoleCommandDescriptor
{
    std::string name;
    std::string description;
};

auto SearchApplicationConfig() -> ApplicationConfig;
auto SearchApplicationServerName() -> std::string;
auto SearchApplicationConsoleCommandDescriptors(uint16 expiryDays) -> std::array<SearchConsoleCommandDescriptor, 2>;

// SearchMainExitCode is the fixed process exit code after searchApp->run().
// Production always returns 0 regardless of run outcome (same polarity as world/connect).
// Go host pure half: searchutil.SearchMain always returns this (slice 6374).
inline constexpr int SearchMainExitCode = 0;
