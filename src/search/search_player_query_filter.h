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

auto BuildSearchPlayerCountQuery(const search_req& request) -> SearchPlayerCountQuery;
auto BuildSearchPlayerQueryFilter(const search_req& request) -> std::string;
