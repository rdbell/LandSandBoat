#pragma once

#include "common/cbasetypes.h"
#include "common/types/maybe.h"

struct SearchAuctionExpirationSettings
{
    bool   enabled         = false;
    uint32 intervalSeconds = 0;
};

struct SearchAuctionExpirationPlan
{
    bool   schedulePeriodicCleanup = false;
    uint32 intervalSeconds         = 0;
};

struct SearchAuctionInitializationSettings
{
    bool   enabled        = false;
    uint16 expirationDays = 0;
};

struct SearchAuctionInitializationPlan
{
    bool   runInitialCleanup = false;
    uint16 expirationDays    = 0;
};

auto BuildSearchAuctionExpirationPlan(const SearchAuctionExpirationSettings& settings) -> SearchAuctionExpirationPlan;
auto BuildSearchAuctionInitializationPlan(const SearchAuctionInitializationSettings& settings) -> SearchAuctionInitializationPlan;
auto SearchAuctionExpirationDays(Maybe<uint16> days) -> uint16;
