#include "test_battle_immunity_1646.h"

#include "map/battle_immunity_capacity.h"

#include <iostream>

namespace
{
using namespace battleimmunityhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle immunity 1646 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleImmunity1646SelfTests() -> bool
{
    bool ok = true;

    // --- type pins ---
    ok = expect(TypePC == 0x01 && TypeNPC == 0x02 && TypeMob == 0x04 && TypePet == 0x08, "type pins low") && ok;
    ok = expect(TypeShip == 0x10 && TypeTrust == 0x20 && TypeFellow == 0x40, "type pins high") && ok;

    // --- IsMobOrPet ---
    ok = expect(IsMobOrPet(TypeMob) && IsMobOrPet(TypePet), "mob/pet true") && ok;
    ok = expect(!IsMobOrPet(0) && !IsMobOrPet(TypePC) && !IsMobOrPet(TypeNPC), "non mob/pet low") && ok;
    ok = expect(!IsMobOrPet(TypeShip) && !IsMobOrPet(TypeTrust) && !IsMobOrPet(TypeFellow), "non mob/pet high") && ok;
    ok = expect(!IsMobOrPet(static_cast<std::uint8_t>(TypeMob | TypePet)), "combined mask not exact") && ok;

    // --- PC / non-mob always false ---
    constexpr std::uint32_t fullMask = 0xFFFFFFFFu;
    constexpr std::uint32_t stun     = 0x00000008u;
    ok = expect(!HasImmunity(TypePC, fullMask, stun), "PC always false") && ok;
    ok = expect(!HasImmunity(TypeNPC, fullMask, stun), "NPC always false") && ok;
    ok = expect(!HasImmunity(TypeTrust, fullMask, stun), "Trust always false") && ok;
    ok = expect(!HasImmunity(TypeFellow, fullMask, stun), "Fellow always false") && ok;
    ok = expect(!HasImmunity(0, fullMask, stun), "None always false") && ok;

    // --- mob match / no match ---
    constexpr std::uint32_t silence = 0x00000010u;
    constexpr std::uint32_t bind    = 0x00000004u;
    ok = expect(HasImmunity(TypeMob, stun, stun), "mob stun match") && ok;
    ok = expect(!HasImmunity(TypeMob, stun, silence), "mob stun vs silence") && ok;
    ok = expect(!HasImmunity(TypeMob, 0, stun), "mob empty mask") && ok;
    ok = expect(!HasImmunity(TypeMob, fullMask, 0), "mob imID zero") && ok;

    // --- pet match / no match ---
    constexpr std::uint32_t gravity = 0x00000002u;
    constexpr std::uint32_t poison  = 0x00000100u;
    ok = expect(HasImmunity(TypePet, gravity, gravity), "pet gravity match") && ok;
    ok = expect(!HasImmunity(TypePet, gravity, poison), "pet gravity vs poison") && ok;
    ok = expect(HasImmunity(TypePet, gravity | poison, poison), "pet multi mask single imID") && ok;

    // --- multi-bit: any overlapping bits is true ---
    constexpr std::uint32_t mask = stun | silence; // 0x18
    ok = expect(HasImmunity(TypeMob, mask, stun), "partial single-bit") && ok;
    ok = expect(HasImmunity(TypeMob, mask, stun | bind), "partial multi-bit overlap") && ok;
    ok = expect(HasImmunity(TypeMob, mask, stun | silence), "full multi-bit") && ok;
    ok = expect(!HasImmunity(TypeMob, mask, bind), "no overlap multi-bit") && ok;
    ok = expect(HasImmunity(TypePet, mask, silence | bind), "pet partial overlap") && ok;

    constexpr std::uint32_t petrify = 0x00010000u;
    constexpr std::uint32_t plague  = 0x00020000u;
    ok = expect(HasImmunity(TypeMob, petrify | plague, plague), "high-bit partial") && ok;
    ok = expect(!HasImmunity(TypeMob, petrify | plague, stun), "high-bit vs low-bit") && ok;

    return ok;
}
