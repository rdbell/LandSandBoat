#pragma once

#include "common/earth_time.h"
#include "common/vana_time.h"

// Clock-derived inputs shared by one map time_server invocation.
namespace timeservertickinputhelpers
{

struct Input
{
    earth_time::time_point    earthTime;
    uint32                    jstHour;
    uint32                    jstWeekday;
    vanadiel_time::time_point vanaTime;
    uint32                    vanaHour;
    vanadiel_time::TOTD       vanaTotd;
};

inline auto MakeInput(const earth_time::time_point earthTime) -> Input
{
    const auto vanaTime = vanadiel_time::from_earth_time(earthTime);
    return {
        .earthTime  = earthTime,
        .jstHour    = earth_time::jst::get_hour(earthTime),
        .jstWeekday = earth_time::jst::get_weekday(earthTime),
        .vanaTime   = vanaTime,
        .vanaHour   = vanadiel_time::get_hour(vanaTime),
        .vanaTotd   = vanadiel_time::get_totd(vanaTime),
    };
}

} // namespace timeservertickinputhelpers
