#include "test_battle_trait_1655.h"

#include "map/battle_trait_capacity.h"

#include <iostream>
#include <vector>

namespace
{
using namespace battletraithelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle trait 1655 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleTrait1655SelfTests() -> bool
{
    bool ok = true;

    // --- empty / nil-equivalent ---
    const std::vector<std::uint16_t> empty{};
    ok = expect(!HasTrait(empty, 0), "empty want 0") && ok;
    ok = expect(!HasTrait(empty, 15), "empty want 15") && ok;

    // --- match first / last / middle ---
    const std::vector<std::uint16_t> ids{ 1, 15, 42, 100 };
    ok = expect(HasTrait(ids, 1), "first element") && ok;
    ok = expect(HasTrait(ids, 100), "last element") && ok;
    ok = expect(HasTrait(ids, 15), "middle double attack") && ok;
    ok = expect(HasTrait(ids, 42), "middle 42") && ok;

    // --- no match ---
    ok = expect(!HasTrait(ids, 0), "miss 0") && ok;
    ok = expect(!HasTrait(ids, 2), "miss 2") && ok;
    ok = expect(!HasTrait(ids, 0xFFFF), "miss 0xFFFF") && ok;

    // --- single element ---
    const std::vector<std::uint16_t> one{ 7 };
    ok = expect(HasTrait(one, 7), "single match") && ok;
    ok = expect(!HasTrait(one, 8), "single miss") && ok;
    const std::vector<std::uint16_t> zeroID{ 0 };
    ok = expect(HasTrait(zeroID, 0), "zero ID match") && ok;

    // --- duplicates: presence only ---
    const std::vector<std::uint16_t> dups{ 3, 15, 15, 3 };
    ok = expect(HasTrait(dups, 15), "dup 15") && ok;
    ok = expect(HasTrait(dups, 3), "dup 3") && ok;
    ok = expect(!HasTrait(dups, 99), "dup miss 99") && ok;

    // --- order independence of presence ---
    const std::vector<std::uint16_t> a{ 20, 10, 30 };
    const std::vector<std::uint16_t> b{ 30, 20, 10 };
    ok = expect(HasTrait(a, 10) && HasTrait(b, 10), "order 10") && ok;
    ok = expect(HasTrait(a, 20) && HasTrait(b, 20), "order 20") && ok;
    ok = expect(HasTrait(a, 30) && HasTrait(b, 30), "order 30") && ok;
    ok = expect(!HasTrait(a, 40) && !HasTrait(b, 40), "order miss 40") && ok;

    // --- table: common trait IDs ---
    constexpr std::uint16_t doubleAttack = 15;
    constexpr std::uint16_t ambush       = 68;
    const std::vector<std::uint16_t> withDA{ 1, doubleAttack, 20 };
    const std::vector<std::uint16_t> noDA{ 1, 20 };
    const std::vector<std::uint16_t> onlyAmbush{ ambush };
    const std::vector<std::uint16_t> fullRange{ 0, 1, 0xFFFF };
    const std::vector<std::uint16_t> nearFull{ 0, 1, 0xFFFE };

    ok = expect(HasTrait(withDA, doubleAttack), "hit double attack") && ok;
    ok = expect(!HasTrait(noDA, doubleAttack), "miss double attack") && ok;
    ok = expect(HasTrait(onlyAmbush, ambush), "only ambush hit") && ok;
    ok = expect(!HasTrait(onlyAmbush, doubleAttack), "only ambush miss DA") && ok;
    ok = expect(HasTrait(fullRange, 0xFFFF), "full uint16 hit") && ok;
    ok = expect(!HasTrait(nearFull, 0xFFFF), "full uint16 miss") && ok;

    return ok;
}
