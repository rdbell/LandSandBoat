#pragma once

#include <string>

struct search_req;

auto BuildSearchPlayerQueryFilter(const search_req& request) -> std::string;
