#pragma once

struct SearchEntity;
struct search_req;

auto SearchPlayerMatchesRequest(const SearchEntity& player, const search_req& request) -> bool;
