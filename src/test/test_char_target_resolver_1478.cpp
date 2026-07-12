#include "test_char_target_resolver_1478.h"

#include "map/char_target_resolver_capacity.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
auto Run(bool found, bool player, bool alive, std::uint16_t flags, bool blocked, bool owner, bool fallback, std::vector<std::string>& calls)
    -> chartargetresolverhelpers::Decision
{
    return chartargetresolverhelpers::Apply(
        found, player, alive, flags,
        [&]() { calls.emplace_back("aid"); return blocked; },
        [&]() { calls.emplace_back("owner"); return owner; },
        [&]() { calls.emplace_back("fallback"); return fallback; });
}
} // namespace

auto runCharTargetResolver1478SelfTests() -> bool
{
    using D = chartargetresolverhelpers::Decision;
    std::vector<std::string> calls{};
    bool ok = Run(true, true, true, 0, true, true, false, calls) == D::Blocked && calls == std::vector<std::string>{ "aid" };
    calls.clear();
    ok = Run(true, true, true, 0, false, true, false, calls) == D::Accept && calls == std::vector<std::string>{ "aid", "owner" } && ok;
    calls.clear();
    ok = Run(true, false, false, 0x20, false, true, false, calls) == D::Accept && calls == std::vector<std::string>{ "owner" } && ok;
    calls.clear();
    ok = Run(true, false, false, 0, false, true, false, calls) == D::CannotOnThatTarget && ok;
    calls.clear();
    ok = Run(true, false, true, 0, false, false, false, calls) == D::AlreadyClaimed && ok;
    calls.clear();
    ok = Run(false, false, false, 0, false, false, true, calls) == D::CannotOnThatTarget && calls == std::vector<std::string>{ "fallback" } && ok;
    calls.clear();
    ok = Run(false, false, false, 0, false, false, false, calls) == D::CannotAttack && ok;
    if (!ok)
    {
        std::cerr << "char target resolver 1478 self-test failed\n";
    }
    return ok;
}
