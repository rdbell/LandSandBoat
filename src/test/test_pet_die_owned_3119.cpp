#include "test_pet_die_owned_3119.h"

#include "map/pet_detach_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet ShouldDieOwnedPet 3119 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline DetachPet owned-pet Die formula for dual-wire cross-check
// (slice 3119):
//   !isDead
auto inlineShouldDieOwnedPet(const bool isDead) -> bool
{
    return !isDead;
}

} // namespace

// Pure dual-wire expansion for petdetachhelpers::ShouldDieOwnedPet
// (!isDead on DetachPet TYPE_PET / OwnedPet branch; slice 3119). Dense 2¹
// boolean space.
auto runPetDieOwned3119SelfTests() -> bool
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
    ok = expect(ShouldDieOwnedPet(false), "residual alive owned pet → Die") && ok;
    ok = expect(!ShouldDieOwnedPet(true), "residual already dead → skip Die") && ok;

    const struct
    {
        bool        isDead;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles (full dense 2¹).
        { false, true, "alive owned pet dies" },
        { true, false, "already dead skips Die" },

        // Residual 1626 pins.
        { false, true, "residual ShouldDieOwnedPet(false)" },
        { true, false, "residual ShouldDieOwnedPet(true)" },

        // Polarity / dual-wire stability repeats.
        { false, true, "polarity Die when alive" },
        { true, false, "polarity skip when dead" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDieOwnedPet(c.isDead);
        const bool inlineF = inlineShouldDieOwnedPet(c.isDead);
        const bool wantPin = !c.isDead;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDieOwnedPet dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDieOwnedPet == pin formula !isDead") && ok;
    }

    // Pin composition: Die iff !isDead.
    ok = expect(ShouldDieOwnedPet(false), "isDead false must Die") && ok;
    ok = expect(!ShouldDieOwnedPet(true), "isDead true must not Die") && ok;

    // Dense compose: full 2¹ boolean space.
    for (const bool isDead : { false, true })
    {
        const bool got  = ShouldDieOwnedPet(isDead);
        const bool want = !isDead;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldDieOwnedPet(isDead), "compose free == inline") && ok;
    }

    // Explicit polarity: Die iff NOT already dead.
    for (const bool isDead : { false, true })
    {
        const bool got = ShouldDieOwnedPet(isDead);
        ok             = expect(got == !isDead, "polarity: Die == !isDead") && ok;
        ok             = expect(!(got && isDead), "polarity: must not Die when isDead true") && ok;
        ok             = expect(!(!got && !isDead), "polarity: must Die when isDead false") && ok;
    }

    // Host-style inject poles: PPet->isDead() as bool on DetachPet OwnedPet.
    for (const bool isDead : { false, true })
    {
        const bool shouldDie = ShouldDieOwnedPet(isDead);
        ok                   = expect(shouldDie == !isDead, "host inject dual-wire polarity") && ok;
        ok                   = expect(shouldDie == inlineShouldDieOwnedPet(isDead),
                    "host inject free == inline") &&
             ok;
        // Positive form: alive == !isDead (avoid De Morgan rewrite of !!isDead).
        const bool alive = !isDead;
        ok               = expect(shouldDie == alive, "host inject shouldDie == alive") && ok;
        ok               = expect(!(shouldDie && isDead), "Die only when !isDead") && ok;
        ok               = expect(!(!shouldDie && !isDead), "!isDead must Die") && ok;
    }

    // Production DetachPet OwnedPet path semantics:
    // !isDead → Die; else skip.
    ok = expect(ShouldDieOwnedPet(false), "DetachPet alive owned pet → Die path") && ok;
    ok = expect(!ShouldDieOwnedPet(true), "DetachPet already dead → skip-Die path") && ok;

    // Sibling residual ClassifyDetachPet (1626) is orthogonal: OwnedPet branch
    // selected before the Die gate; free Die polarity does not depend on
    // objtype classification.
    ok = expect(ClassifyDetachPet(EntityTypePET) == DetachKind::OwnedPet,
                "residual ClassifyDetachPet TYPE_PET → OwnedPet") &&
         ok;
    ok = expect(ClassifyDetachPet(EntityTypeMOB) == DetachKind::CharmedMob,
                "residual ClassifyDetachPet TYPE_MOB → CharmedMob") &&
         ok;
    for (const bool isDead : { false, true })
    {
        ok = expect(ShouldDieOwnedPet(isDead) == !isDead, "die vs classify compose") && ok;
        const bool owned    = ClassifyDetachPet(EntityTypePET) == DetachKind::OwnedPet;
        const bool wouldDie = owned && !isDead;
        const bool gotDie   = owned && ShouldDieOwnedPet(isDead);
        ok                  = expect(gotDie == wouldDie, "DetachPet OwnedPet path both gates") && ok;
        const bool charmed  = ClassifyDetachPet(EntityTypeMOB) == DetachKind::CharmedMob;
        ok                  = expect(charmed, "compose: TYPE_MOB must classify CharmedMob") && ok;
    }

    // Sibling residual ShouldClearAvatarPerpetuation (1626) is orthogonal:
    // runs after Die gate on OwnedPet; leave alone.
    ok = expect(ShouldClearAvatarPerpetuation(PetTypeAvatar),
                "sibling residual avatar perpetuation clear") &&
         ok;
    ok = expect(!ShouldClearAvatarPerpetuation(1),
                "sibling residual non-avatar skip perpetuation clear") &&
         ok;
    for (const bool isDead : { false, true })
    {
        ok = expect(ShouldDieOwnedPet(isDead) == !isDead, "die vs avatar compose") && ok;
        ok = expect(ShouldClearAvatarPerpetuation(PetTypeAvatar),
                    "die vs avatar: avatar still clears") &&
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

    // Explicit dual-wire poles: free == !isDead for both bools.
    for (const bool isDead : { false, true })
    {
        const bool got = ShouldDieOwnedPet(isDead);
        ok             = expect(got == !isDead, "host inject dual-wire polarity poles") && ok;
        ok             = expect(got == inlineShouldDieOwnedPet(isDead),
                    "host inject free == inline poles") &&
             ok;
    }

    return ok;
}
