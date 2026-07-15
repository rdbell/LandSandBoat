#include "test_using_h2h_1666.h"

#include "map/using_h2h_capacity.h"

#include <iostream>

namespace
{
using namespace usingh2hhelpers;

constexpr std::uint8_t skillDagger = 2;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "using h2h 1666 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runUsingH2H1666SelfTests() -> bool
{
    bool ok = true;

    // --- constant pin ---
    ok = expect(SkillIndexH2H == 1, "SkillIndexH2H is 1") && ok;

    // --- PC branch ---
    ok = expect(IsPCUsingH2H(false, 0, SkillIndexH2H), "pc bare hands") && ok;
    ok = expect(IsPCUsingH2H(false, skillDagger, SkillIndexH2H), "pc bare ignores skill") && ok;
    ok = expect(IsPCUsingH2H(true, SkillIndexH2H, SkillIndexH2H), "pc main h2h") && ok;
    ok = expect(!IsPCUsingH2H(true, skillDagger, SkillIndexH2H), "pc main dagger") && ok;
    ok = expect(!IsPCUsingH2H(true, 0, SkillIndexH2H), "pc main skill zero") && ok;
    ok = expect(IsPCUsingH2H(true, 7, 7), "pc custom h2h match") && ok;
    ok = expect(!IsPCUsingH2H(true, 7, 8), "pc custom h2h mismatch") && ok;

    // --- mob/pet branch ---
    ok = expect(!IsMobPetUsingH2H(false, 0, SkillIndexH2H), "mob empty main") && ok;
    ok = expect(!IsMobPetUsingH2H(false, SkillIndexH2H, SkillIndexH2H), "mob empty ignores skill") && ok;
    ok = expect(IsMobPetUsingH2H(true, SkillIndexH2H, SkillIndexH2H), "mob main h2h") && ok;
    ok = expect(!IsMobPetUsingH2H(true, skillDagger, SkillIndexH2H), "mob main dagger") && ok;

    // --- resolve: PC ---
    ok = expect(IsUsingH2H(true, false, false, 0, SkillIndexH2H), "resolve pc bare") && ok;
    ok = expect(IsUsingH2H(true, true, false, 0, SkillIndexH2H), "resolve pc bare mob flag ignored") && ok;
    ok = expect(IsUsingH2H(true, false, true, SkillIndexH2H, SkillIndexH2H), "resolve pc h2h") && ok;
    ok = expect(!IsUsingH2H(true, false, true, skillDagger, SkillIndexH2H), "resolve pc sword") && ok;
    ok = expect(!IsUsingH2H(true, true, true, skillDagger, SkillIndexH2H), "resolve pc sword mob flag ignored") && ok;

    // --- resolve: mob/pet ---
    ok = expect(!IsUsingH2H(false, true, false, 0, SkillIndexH2H), "resolve mob empty") && ok;
    ok = expect(!IsUsingH2H(false, true, false, SkillIndexH2H, SkillIndexH2H), "resolve mob empty skill h2h") && ok;
    ok = expect(IsUsingH2H(false, true, true, SkillIndexH2H, SkillIndexH2H), "resolve mob h2h") && ok;
    ok = expect(!IsUsingH2H(false, true, true, skillDagger, SkillIndexH2H), "resolve mob sword") && ok;
    ok = expect(IsUsingH2H(false, true, true, SkillIndexH2H, SkillIndexH2H), "resolve pet h2h") && ok;

    // --- resolve: other (NPC) ---
    ok = expect(!IsUsingH2H(false, false, false, 0, SkillIndexH2H), "npc empty") && ok;
    ok = expect(!IsUsingH2H(false, false, true, SkillIndexH2H, SkillIndexH2H), "npc with h2h main") && ok;
    ok = expect(!IsUsingH2H(false, false, true, skillDagger, SkillIndexH2H), "npc with sword") && ok;

    // --- table scenarios ---
    ok = expect(IsUsingH2H(true, false, false, 0, SkillIndexH2H), "table pc bare hands") && ok;
    ok = expect(IsUsingH2H(true, false, true, SkillIndexH2H, SkillIndexH2H), "table pc h2h weapon") && ok;
    ok = expect(!IsUsingH2H(true, false, true, skillDagger, SkillIndexH2H), "table pc sword") && ok;
    ok = expect(!IsUsingH2H(false, true, false, 0, SkillIndexH2H), "table mob empty main") && ok;
    ok = expect(IsUsingH2H(false, true, true, SkillIndexH2H, SkillIndexH2H), "table mob h2h") && ok;
    ok = expect(!IsUsingH2H(false, true, true, skillDagger, SkillIndexH2H), "table mob sword") && ok;
    ok = expect(IsUsingH2H(false, true, true, SkillIndexH2H, SkillIndexH2H), "table pet h2h") && ok;
    ok = expect(!IsUsingH2H(false, true, false, SkillIndexH2H, SkillIndexH2H), "table pet empty") && ok;
    ok = expect(!IsUsingH2H(false, false, true, SkillIndexH2H, SkillIndexH2H), "table npc always false") && ok;

    return ok;
}
