#pragma once

#include "common/cbasetypes.h"

#include <string>

struct search_req;

struct SearchPlayerCountQuery
{
    std::string sql        = {};
    bool        filtersJob = false;
    uint8       jobID      = 0;
};

struct SearchPartyListQuery
{
    std::string sql         = {};
    uint32      firstParam  = 0;
    uint32      secondParam = 0;
};

auto BuildSearchPlayerCountQuery(const search_req& request) -> SearchPlayerCountQuery;
auto BuildSearchPlayerListQuery(const std::string& filterQuery) -> std::string;
auto BuildSearchPartyListQuery(uint32 partyID, uint32 allianceID) -> SearchPartyListQuery;
auto BuildSearchPlayerQueryFilter(const search_req& request) -> std::string;
