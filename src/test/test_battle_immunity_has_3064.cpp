#include "test_battle_immunity_has_3064.h"

#include "map/battle_immunity_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle immunity HasImmunity 3064 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CBattleEntity::hasImmunity for dual-wire cross-check:
//   if !IsMobOrPet(objtype): false
//   else (immunityMask & imID) != 0
auto inlineHasImmunity(const std::uint8_t  objtype,
                       const std::uint32_t immunityMask,
                       const std::uint32_t imID) -> bool
{
    if (!(objtype == battleimmunityhelpers::TypeMob || objtype == battleimmunityhelpers::TypePet))
    {
        return false;
    }
    return (immunityMask & imID) != 0;
}

// Compact dual-wire pin matching C++ capacity / Go pinHasImmunity3064:
//   IsMobOrPet(objtype) && (immunityMask & imID) != 0
auto pinHasImmunity(const std::uint8_t  objtype,
                    const std::uint32_t immunityMask,
                    const std::uint32_t imID) -> bool
{
    return battleimmunityhelpers::IsMobOrPet(objtype) && (immunityMask & imID) != 0;
}

} // namespace

// Pure dual-wire expansion for battleimmunityhelpers::HasImmunity
// (CBattleEntity::hasImmunity; OmegaXI internal/battleimmunity; slice 3064).
//
// Coverage:
//   - mob/pet true with overlapping bits
//   - non-mob false even with full mask
//   - zero mask → false
//   - multi-bit overlap (any non-zero AND)
//   - free == inline pin matching C++
auto runBattleImmunityHas3064SelfTests() -> bool
{
    using battleimmunityhelpers::HasImmunity;
    using battleimmunityhelpers::IsMobOrPet;
    using battleimmunityhelpers::TypeFellow;
    using battleimmunityhelpers::TypeMob;
    using battleimmunityhelpers::TypeNPC;
    using battleimmunityhelpers::TypePC;
    using battleimmunityhelpers::TypePet;
    using battleimmunityhelpers::TypeShip;
    using battleimmunityhelpers::TypeTrust;

    bool ok = true;

    constexpr std::uint32_t stun    = 0x00000008u;
    constexpr std::uint32_t silence = 0x00000010u;
    constexpr std::uint32_t bind    = 0x00000004u;
    constexpr std::uint32_t gravity = 0x00000002u;
    constexpr std::uint32_t poison  = 0x00000100u;
    constexpr std::uint32_t petrify = 0x00010000u;
    constexpr std::uint32_t plague  = 0x00020000u;
    constexpr std::uint32_t full    = 0xFFFFFFFFu;

    // Pin constants match ENTITYTYPE.
    ok = expect(TypePC == 0x01 && TypeNPC == 0x02 && TypeMob == 0x04 && TypePet == 0x08, "pin types low") && ok;
    ok = expect(TypeShip == 0x10 && TypeTrust == 0x20 && TypeFellow == 0x40, "pin types high") && ok;

    // Residual 1646 pins still hold under dual-wire.
    ok = expect(!HasImmunity(TypePC, full, stun), "residual: PC always false") && ok;
    ok = expect(HasImmunity(TypeMob, stun, stun), "residual: mob stun match") && ok;
    ok = expect(HasImmunity(TypePet, gravity, gravity), "residual: pet gravity match") && ok;
    ok = expect(IsMobOrPet(TypeMob) && IsMobOrPet(TypePet), "residual: IsMobOrPet sibling true") && ok;
    ok = expect(!IsMobOrPet(TypePC) && !IsMobOrPet(static_cast<std::uint8_t>(TypeMob | TypePet)),
                "residual: IsMobOrPet sibling non-exact false") &&
         ok;

    const struct
    {
        std::uint8_t  objtype;
        std::uint32_t mask;
        std::uint32_t imID;
        bool          want;
        const char*   label;
    } cases[] = {
        // Mob true with overlapping bits.
        { TypeMob, stun, stun, true, "mob exact single-bit overlap" },
        { TypeMob, stun | silence, stun, true, "mob multi-mask single imID" },
        { TypeMob, stun | silence, silence, true, "mob multi-mask silence overlap" },
        { TypeMob, stun | silence, stun | bind, true, "mob partial multi-bit query" },
        { TypeMob, stun | silence, stun | silence, true, "mob full multi-bit query" },
        { TypeMob, petrify | plague, plague, true, "mob high-bit partial" },
        { TypeMob, full, stun, true, "mob full mask single bit" },

        // Pet true with overlapping bits.
        { TypePet, gravity, gravity, true, "pet exact single-bit overlap" },
        { TypePet, gravity | poison, poison, true, "pet multi-mask single imID" },
        { TypePet, stun | silence, silence | bind, true, "pet partial multi-bit overlap" },
        { TypePet, full, poison, true, "pet full mask single bit" },
        { TypePet, petrify | plague, petrify, true, "pet high-bit partial" },

        // Non-mob always false (even with full mask / matching imID).
        { TypePC, full, stun, false, "PC always false" },
        { TypeNPC, full, stun, false, "NPC always false" },
        { TypeShip, full, stun, false, "Ship always false" },
        { TypeTrust, full, stun, false, "Trust always false" },
        { TypeFellow, full, stun, false, "Fellow always false" },
        { 0, full, stun, false, "None always false" },
        { static_cast<std::uint8_t>(TypeMob | TypePet), full, stun, false, "combined Mob|Pet not exact" },
        { 0x0C, full, stun, false, "0x0C combined not exact" },
        { 0xFF, full, stun, false, "0xFF not exact MOB or PET" },

        // Zero mask → false for mob/pet.
        { TypeMob, 0, stun, false, "mob zero mask" },
        { TypePet, 0, poison, false, "pet zero mask" },
        { TypeMob, 0, 0, false, "mob zero mask zero imID" },
        { TypePet, 0, full, false, "pet zero mask full imID" },

        // Zero imID → false even with full mask.
        { TypeMob, full, 0, false, "mob imID zero" },
        { TypePet, full, 0, false, "pet imID zero" },

        // Multi-bit no overlap → false.
        { TypeMob, stun, silence, false, "mob stun vs silence no overlap" },
        { TypeMob, stun | silence, bind, false, "mob multi-mask no overlapping bit" },
        { TypePet, gravity, poison, false, "pet gravity vs poison no overlap" },
        { TypeMob, petrify | plague, stun, false, "mob high-bit vs low-bit" },
        { TypePet, stun | silence, bind, false, "pet multi-bit no overlap" },
    };

    for (const auto& c : cases)
    {
        const bool got     = HasImmunity(c.objtype, c.mask, c.imID);
        const bool inlineF = inlineHasImmunity(c.objtype, c.mask, c.imID);
        const bool pinF    = pinHasImmunity(c.objtype, c.mask, c.imID);
        const bool pure    = IsMobOrPet(c.objtype) && (c.mask & c.imID) != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == IsMobOrPet && AND") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline formula") && ok;
        ok = expect(got == pinF, "dual-wire free == C++ pin formula") && ok;
    }

    // Free == inline pin matching C++ across residual poles.
    ok = expect(HasImmunity(TypeMob, stun, stun) == pinHasImmunity(TypeMob, stun, stun), "free == pin for mob exact") && ok;
    ok = expect(HasImmunity(TypePet, gravity, gravity) == pinHasImmunity(TypePet, gravity, gravity), "free == pin for pet exact") && ok;
    ok = expect(HasImmunity(TypePC, full, stun) == pinHasImmunity(TypePC, full, stun), "free == pin for PC") && ok;
    ok = expect(HasImmunity(TypeMob, 0, stun) == pinHasImmunity(TypeMob, 0, stun), "free == pin for zero mask") && ok;
    ok = expect(HasImmunity(TypeMob, stun | silence, stun | bind) ==
                    pinHasImmunity(TypeMob, stun | silence, stun | bind),
                "free == pin for multi-bit partial") &&
         ok;

    // Compose over host-style poles: entity types × mask/imID pairs.
    const std::uint8_t objtypes[] = {
        0, TypePC, TypeNPC, TypeMob, TypePet, TypeShip, TypeTrust, TypeFellow,
        static_cast<std::uint8_t>(TypeMob | TypePet), 0x0C, 0xFF, 0x05, 0x09,
    };
    const std::uint32_t masks[] = {
        0, 0x00000001u, stun, silence, stun | silence, poison, petrify, petrify | plague, full,
    };
    const std::uint32_t imIDs[] = {
        0, 0x00000001u, bind, stun, silence, stun | silence, bind | stun, poison, plague, full,
    };

    for (const auto ot : objtypes)
    {
        for (const auto mask : masks)
        {
            for (const auto imID : imIDs)
            {
                const bool got  = HasImmunity(ot, mask, imID);
                const bool want = IsMobOrPet(ot) && (mask & imID) != 0;
                ok = expect(got == want, "compose free == formula") && ok;
                ok = expect(got == inlineHasImmunity(ot, mask, imID), "compose free == inline") && ok;
                ok = expect(got == pinHasImmunity(ot, mask, imID), "compose free == pin") && ok;
            }
        }
    }

    // Dense multi-bit overlap grid for mob/pet only.
    const std::uint32_t multiMasks[] = {
        0, stun, silence, bind, stun | silence, stun | bind, silence | bind, stun | silence | bind, full,
    };
    for (const auto ot : { TypeMob, TypePet })
    {
        for (const auto mask : multiMasks)
        {
            for (const auto imID : multiMasks)
            {
                const bool got  = HasImmunity(ot, mask, imID);
                const bool want = (mask & imID) != 0;
                ok = expect(got == want, "dense multi-bit free == AND") && ok;
            }
        }
    }

    return ok;
}
