#include "search_engine_config.h"

auto BuildSearchAuctionExpirationPlan(const SearchAuctionExpirationSettings& settings) -> SearchAuctionExpirationPlan
{
    if (!settings.enabled)
    {
        return {};
    }

    return SearchAuctionExpirationPlan{
        .schedulePeriodicCleanup = true,
        .intervalSeconds         = settings.intervalSeconds,
    };
}

auto BuildSearchAuctionInitializationPlan(const SearchAuctionInitializationSettings& settings) -> SearchAuctionInitializationPlan
{
    if (!settings.enabled)
    {
        return {};
    }

    return SearchAuctionInitializationPlan{
        .runInitialCleanup = true,
        .expirationDays    = settings.expirationDays,
    };
}

auto SearchAuctionExpirationDays(const Maybe<uint16> days) -> uint16
{
    return days.value_or(0);
}
