#include "search_application_config.h"

#include <fmt/format.h>

auto SearchApplicationConfig() -> ApplicationConfig
{
    return ApplicationConfig{
        .serverName = SearchApplicationServerName(),
        .arguments  = {},
    };
}

auto SearchApplicationServerName() -> std::string
{
    return "search";
}

auto SearchApplicationConsoleCommandDescriptors(const uint16 expiryDays) -> std::array<SearchConsoleCommandDescriptor, 2>
{
    return {
        SearchConsoleCommandDescriptor{
            .name        = "ah_cleanup",
            .description = fmt::format("AH task to return items older than {} days", expiryDays),
        },
        SearchConsoleCommandDescriptor{
            .name        = "expire_all",
            .description = "Force-expire all items on the AH, returning to sender",
        },
    };
}
