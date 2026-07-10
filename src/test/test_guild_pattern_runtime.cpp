#include "test_guild_pattern_runtime.h"

#include "map/utils/guildutils.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{

struct Trace
{
    std::uint32_t            updateDay{};
    std::uint32_t            storedPattern{};
    std::uint8_t             randomPattern{};
    std::vector<std::string> events;
    std::vector<std::uint8_t> guildPatterns;

    auto get(const std::string& name) -> std::uint32_t
    {
        events.emplace_back("get:" + name);
        return name == "[GUILD]pattern_update" ? updateDay : storedPattern;
    }

    void set(const std::string& name, const std::uint32_t value)
    {
        events.emplace_back("set:" + name);
        if (name == "[GUILD]pattern_update")
        {
            updateDay = value;
        }
        else
        {
            storedPattern = value;
        }
    }
};

auto runPattern(Trace& trace, const std::uint32_t jstDay, const std::size_t guildCount) -> std::uint8_t
{
    const auto storedDay = trace.get("[GUILD]pattern_update");
    trace.events.emplace_back("random:8");
    const auto storedPattern = storedDay == jstDay ? trace.get("[GUILD]pattern") : 0;
    const auto decision = guildutils::detail::SelectPattern(storedDay, jstDay, trace.randomPattern, storedPattern);
    const auto pattern = decision.pattern;
    if (decision.persist)
    {
        trace.set("[GUILD]pattern_update", jstDay);
        trace.set("[GUILD]pattern", pattern);
    }

    for (std::size_t index = 0; index < guildCount; ++index)
    {
        trace.events.emplace_back("guild:" + std::to_string(index));
        trace.guildPatterns.emplace_back(pattern);
    }
    return pattern;
}

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "guild pattern runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto testNewDayWritesThenApplies() -> bool
{
    Trace trace{ .updateDay = 40, .storedPattern = 6, .randomPattern = 3 };
    const auto pattern = runPattern(trace, 41, 2);
    const std::vector<std::string> expected{
        "get:[GUILD]pattern_update", "random:8", "set:[GUILD]pattern_update",
        "set:[GUILD]pattern", "guild:0", "guild:1"
    };
    return expect(pattern == 3, "new-day pattern") &&
           expect(trace.updateDay == 41 && trace.storedPattern == 3, "new-day persisted values") &&
           expect(trace.events == expected, "new-day side-effect order") &&
           expect(trace.guildPatterns == std::vector<std::uint8_t>{ 3, 3 }, "new-day guild applications");
}

auto testSameDayConsumesRandomAndNarrowsReload() -> bool
{
    Trace trace{ .updateDay = 200, .storedPattern = 261, .randomPattern = 7 };
    const auto pattern = runPattern(trace, 200, 1);
    const std::vector<std::string> expected{
        "get:[GUILD]pattern_update", "random:8", "get:[GUILD]pattern", "guild:0"
    };
    return expect(pattern == 5, "same-day narrowed persisted pattern") &&
           expect(trace.events == expected, "same-day side-effect order") &&
           expect(trace.storedPattern == 261, "same-day no writes") &&
           expect(trace.guildPatterns == std::vector<std::uint8_t>{ 5 }, "same-day guild application");
}

} // namespace

auto runGuildPatternRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok      = testNewDayWritesThenApplies() && ok;
    ok      = testSameDayConsumesRandomAndNarrowsReload() && ok;
    return ok;
}
