#pragma once

#include "traits_source_capacity.h"

#include <cstdint>
#include <vector>

namespace traitrosterhelpers
{
struct Source
{
    uint8_t job{};
    uint8_t level{};

    constexpr auto operator==(const Source&) const -> bool = default;
};

struct Plan
{
    std::vector<Source> addTraitSources{};
};

inline auto PlanFor(const traitssourcehelpers::Plan& source) -> Plan
{
    return {
        .addTraitSources = {
            { .job = source.mainJob, .level = source.mainLevel },
            { .job = source.subJob, .level = source.subLevel },
        },
    };
}
} // namespace traitrosterhelpers
