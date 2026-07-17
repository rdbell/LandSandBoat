#include "test_pet_clear_avatar_3137.h"

#include "map/pet_detach_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet ShouldClearAvatarPerpetuation 3137 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DetachPet owned-pet AVATAR_PERPETUATION clear formula for dual-wire
// cross-check (slice 3137):
//   petType == PetTypeAvatar
auto inlineShouldClearAvatarPerpetuation(const std::uint8_t petType) -> bool
{
    return petType == petdetachhelpers::PetTypeAvatar;
}

} // namespace

// Pure dual-wire expansion for petdetachhelpers::ShouldClearAvatarPerpetuation
// (petType == AVATAR on DetachPet TYPE_PET / OwnedPet branch; slice 3137).
// Clear only when petType is PetTypeAvatar (0).
auto runPetClearAvatar3137SelfTests() -> bool
{
    using petdetachhelpers::ClassifyDetachPet;
    using petdetachhelpers::DetachKind;
    using petdetachhelpers::DetachReject;
    using petdetachhelpers::EntityTypeMOB;
    using petdetachhelpers::EntityTypePET;
    using petdetachhelpers::PetTypeAvatar;
    using petdetachhelpers::ShouldClearAvatarPerpetuation;
    using petdetachhelpers::ShouldDieOwnedPet;
    using petdetachhelpers::ValidateDetachPet;

    bool ok = true;

    // Residual 1626 pins still hold under dual-wire.
    ok = expect(ShouldClearAvatarPerpetuation(PetTypeAvatar), "residual avatar → clear") && ok;
    ok = expect(!ShouldClearAvatarPerpetuation(1), "residual wyvern → skip clear") && ok;
    ok = expect(!ShouldClearAvatarPerpetuation(4), "residual automaton → skip clear") && ok;

    const struct
    {
        std::uint8_t petType;
        bool         want;
        const char*  label;
    } cases[] = {
        // Classic dual poles: avatar vs non-avatar PET_TYPE space.
        { PetTypeAvatar, true, "avatar clears" },
        { 1, false, "wyvern skips clear" },
        { 2, false, "jug skips clear" },
        { 3, false, "charmed skips clear" },
        { 4, false, "automaton skips clear" },
        { 5, false, "fellow skips clear" },
        { 6, false, "chocobo skips clear" },
        { 7, false, "luopan skips clear" },

        // Residual 1626 pins.
        { PetTypeAvatar, true, "residual ShouldClearAvatarPerpetuation(Avatar)" },
        { 1, false, "residual ShouldClearAvatarPerpetuation(Wyvern)" },

        // Polarity / dual-wire stability repeats.
        { PetTypeAvatar, true, "polarity clear when Avatar" },
        { 1, false, "polarity skip when non-Avatar" },
        { 0, true, "raw 0 == PetTypeAvatar clears" },
        { 1, false, "raw 1 skips clear" },
        { 255, false, "raw 255 out-of-enum skips clear" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearAvatarPerpetuation(c.petType);
        const bool inlineF = inlineShouldClearAvatarPerpetuation(c.petType);
        const bool wantPin = c.petType == PetTypeAvatar;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldClearAvatarPerpetuation dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldClearAvatarPerpetuation == pin formula petType==Avatar") && ok;
    }

    // Pin composition: clear iff Avatar.
    ok = expect(ShouldClearAvatarPerpetuation(PetTypeAvatar), "PetTypeAvatar must clear") && ok;
    ok = expect(!ShouldClearAvatarPerpetuation(1), "PetTypeWyvern must not clear") && ok;

    // Dense compose: full PET_TYPE enum space (0…7) + out-of-range.
    const std::uint8_t petTypes[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 255 };
    for (const std::uint8_t petType : petTypes)
    {
        const bool got  = ShouldClearAvatarPerpetuation(petType);
        const bool want = petType == PetTypeAvatar;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldClearAvatarPerpetuation(petType), "compose free == inline") && ok;
    }

    // Explicit polarity: clear iff Avatar.
    for (const std::uint8_t petType : petTypes)
    {
        const bool got      = ShouldClearAvatarPerpetuation(petType);
        const bool isAvatar = petType == PetTypeAvatar;
        ok                  = expect(got == isAvatar, "polarity: Clear == (petType==Avatar)") && ok;
        ok                  = expect(!(got && !isAvatar), "polarity: must not clear when non-Avatar") && ok;
        ok                  = expect(!(!got && isAvatar), "polarity: must clear when Avatar") && ok;
    }

    // Host-style inject poles: getPetType() as uint8 on DetachPet OwnedPet.
    for (const std::uint8_t petType : { PetTypeAvatar, static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(7) })
    {
        const bool shouldClear = ShouldClearAvatarPerpetuation(petType);
        ok                     = expect(shouldClear == (petType == PetTypeAvatar), "host inject dual-wire polarity") && ok;
        ok                     = expect(shouldClear == inlineShouldClearAvatarPerpetuation(petType),
                    "host inject free == inline") &&
             ok;
        // Positive form: isAvatar equality.
        const bool isAvatar = petType == PetTypeAvatar;
        ok                  = expect(shouldClear == isAvatar, "host inject shouldClear == isAvatar") && ok;
        ok                  = expect(!(shouldClear && !isAvatar), "clear only when Avatar") && ok;
        ok                  = expect(!(!shouldClear && isAvatar), "Avatar must clear") && ok;
    }

    // Production DetachPet OwnedPet path semantics:
    // Avatar → clear; else skip.
    ok = expect(ShouldClearAvatarPerpetuation(PetTypeAvatar), "DetachPet avatar → clear path") && ok;
    ok = expect(!ShouldClearAvatarPerpetuation(1), "DetachPet wyvern → skip-clear path") && ok;

    // Sibling residual ClassifyDetachPet (1626) is orthogonal: OwnedPet branch
    // selected before the clear gate; free clear polarity does not depend on
    // objtype classification.
    ok = expect(ClassifyDetachPet(EntityTypePET) == DetachKind::OwnedPet,
                "residual ClassifyDetachPet TYPE_PET → OwnedPet") &&
         ok;
    ok = expect(ClassifyDetachPet(EntityTypeMOB) == DetachKind::CharmedMob,
                "residual ClassifyDetachPet TYPE_MOB → CharmedMob") &&
         ok;
    for (const std::uint8_t petType : { PetTypeAvatar, static_cast<std::uint8_t>(1) })
    {
        ok = expect(ShouldClearAvatarPerpetuation(petType) == (petType == PetTypeAvatar),
                    "clear vs classify compose") &&
             ok;
        const bool owned      = ClassifyDetachPet(EntityTypePET) == DetachKind::OwnedPet;
        const bool wouldClear = owned && petType == PetTypeAvatar;
        const bool gotClear   = owned && ShouldClearAvatarPerpetuation(petType);
        ok                    = expect(gotClear == wouldClear, "DetachPet OwnedPet path both gates") && ok;
        const bool charmed    = ClassifyDetachPet(EntityTypeMOB) == DetachKind::CharmedMob;
        ok                    = expect(charmed, "compose: TYPE_MOB must classify CharmedMob") && ok;
    }

    // Sibling dual-wire ShouldDieOwnedPet (3119) is orthogonal: runs just
    // before avatar clear on OwnedPet; leave alone.
    ok = expect(ShouldDieOwnedPet(false), "sibling residual die gate alive → Die") && ok;
    ok = expect(!ShouldDieOwnedPet(true), "sibling residual die gate dead → skip Die") && ok;
    for (const bool isDead : { false, true })
    {
        ok = expect(ShouldDieOwnedPet(isDead) == !isDead, "die vs avatar compose") && ok;
        ok = expect(ShouldClearAvatarPerpetuation(PetTypeAvatar),
                    "die vs avatar: avatar still clears") &&
             ok;
        ok = expect(!ShouldClearAvatarPerpetuation(1),
                    "die vs avatar: wyvern still skips clear") &&
             ok;
    }

    // Sibling residual ValidateDetachPet (1626) is orthogonal preflight:
    // master/pet/PC checks run before branch classification.
    ok = expect(ValidateDetachPet(true, true, true) == DetachReject::OK,
                "residual ValidateDetachPet admit when PC master+pet") &&
         ok;
    ok = expect(ValidateDetachPet(false, true, true) == DetachReject::MasterNull,
                "residual ValidateDetachPet master null") &&
         ok;

    // Explicit dual-wire poles: free == (petType == PetTypeAvatar).
    for (const std::uint8_t petType : { PetTypeAvatar, static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(7), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(255) })
    {
        const bool got = ShouldClearAvatarPerpetuation(petType);
        ok             = expect(got == (petType == PetTypeAvatar), "host inject dual-wire polarity poles") && ok;
        ok             = expect(got == inlineShouldClearAvatarPerpetuation(petType),
                    "host inject free == inline poles") &&
             ok;
    }

    return ok;
}
