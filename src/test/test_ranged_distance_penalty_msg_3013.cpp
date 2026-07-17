#include "test_ranged_distance_penalty_msg_3013.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ShouldApplyDistancePenaltyMessage 3013 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline OnRangedAttack distance-penalty message gate for dual-wire
// cross-check (slice 3013):
//   return isChar && !isCritMessage
auto inlineShouldApplyDistancePenaltyMessage(const bool isChar, const bool isCritMessage) -> bool
{
    return isChar && !isCritMessage;
}

} // namespace

// Pure dual-wire expansion for rangedammohelpers::ShouldApplyDistancePenaltyMessage
// (OnRangedAttack distance-penalty message gate; slice 3013).
auto runRangedDistancePenaltyMsg3013SelfTests() -> bool
{
    using rangedammohelpers::MsgRangedAttackCrit;
    using rangedammohelpers::MsgRangedAttackHit;
    using rangedammohelpers::ShouldApplyDistancePenaltyMessage;

    bool ok = true;

    const struct
    {
        bool        isChar;
        bool        isCritMessage;
        bool        want;
        const char* label;
    } cases[] = {
        // Char + non-crit: apply distance penalty message.
        { true, false, true, "char non-crit → apply dist msg" },
        // Char + crit: keep crit message (no distance overwrite).
        { true, true, false, "char crit → skip dist msg" },
        // Non-char: never apply (TODO host path for mobs/trusts).
        { false, false, false, "non-char non-crit → skip" },
        { false, true, false, "non-char crit → skip" },

        // Residual 1390 pins.
        { true, false, true, "residual char non-crit" },
        { true, true, false, "residual char crit" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyDistancePenaltyMessage(c.isChar, c.isCritMessage);
        const bool inlineF = inlineShouldApplyDistancePenaltyMessage(c.isChar, c.isCritMessage);
        const bool wantPin = c.isChar && !c.isCritMessage;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplyDistancePenaltyMessage dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplyDistancePenaltyMessage == pin formula isChar && !isCritMessage") && ok;
    }

    // Pin composition: isChar / isCritMessage poles.
    ok = expect(ShouldApplyDistancePenaltyMessage(true, false), "char non-crit must apply distance penalty message") && ok;
    ok = expect(!ShouldApplyDistancePenaltyMessage(true, true), "char crit must not overwrite with distance message") && ok;
    ok = expect(!ShouldApplyDistancePenaltyMessage(false, false), "non-char must not apply distance penalty message") && ok;
    ok = expect(!ShouldApplyDistancePenaltyMessage(false, true), "non-char crit must not apply") && ok;

    // Dense compose: isChar × isCritMessage poles.
    for (const bool isChar : { false, true })
    {
        for (const bool isCrit : { false, true })
        {
            const bool got  = ShouldApplyDistancePenaltyMessage(isChar, isCrit);
            const bool want = isChar && !isCrit;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldApplyDistancePenaltyMessage(isChar, isCrit),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Host-style compose poles: OnRangedAttack injects
    // isChar = entity is character,
    // isCritMessage = actionResult.messageID == MsgBasic::RangedAttackCrit.
    // On true, host computes AttackDistancePenalty and overwrites messageID via
    // RangedDistanceMsgID(ResolveRangedDistanceMessage(distancePenalty)).
    const struct
    {
        bool        isChar;
        bool        isCritMessage;
        bool        wantApply;
        const char* label;
    } composeCases[] = {
        { true, false, true, "char hit (non-crit): apply Pummels/Squarely/Hit" },
        { true, true, false, "char crit: keep RangedAttackCrit" },
        { false, false, false, "non-char hit: skip (TODO mobs/trusts)" },
        { false, true, false, "non-char crit: skip" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldApplyDistancePenaltyMessage(c.isChar, c.isCritMessage);
        ok             = expect(got == c.wantApply, c.label) && ok;
        ok             = expect(got == (c.isChar && !c.isCritMessage), "compose free == pin formula") && ok;
        ok             = expect(got == inlineShouldApplyDistancePenaltyMessage(c.isChar, c.isCritMessage),
                    "compose free == inline") &&
             ok;
    }

    // Production inject pin: OnRangedAttack (~3309) compares messageID to
    // MsgBasic::RangedAttackCrit (pin 353).
    ok = expect(MsgRangedAttackCrit == static_cast<uint16>(353), "MsgRangedAttackCrit pin must be 353") && ok;
    ok = expect(MsgRangedAttackHit != MsgRangedAttackCrit, "Hit pin is not Crit") && ok;
    const auto isCritFromMsgID = [](const uint16 messageID) -> bool {
        return messageID == MsgRangedAttackCrit;
    };
    ok = expect(!ShouldApplyDistancePenaltyMessage(true, isCritFromMsgID(MsgRangedAttackCrit)),
                "messageID == Crit pin must skip distance message") &&
         ok;
    ok = expect(ShouldApplyDistancePenaltyMessage(true, isCritFromMsgID(MsgRangedAttackHit)),
                "messageID == Hit pin must apply distance message on char") &&
         ok;

    return ok;
}
