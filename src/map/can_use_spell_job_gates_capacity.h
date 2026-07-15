#pragma once

#include <cstdint>

// Pure PC/Fellow/NPC job-level and requirement gates from spell::CanUseSpell.
// Parity: internal/spell can_use_spell_gates.go (slice 1732).
//
// Reference:
//   src/map/spell.cpp CanUseSpell TYPE_PC/FELLOW/NPC branch (~707–807)
//
//   main: mLevel >= spell.getJob(mJob) then requirement checks
//   sub:  sLevel >= spell.getJob(sJob) && !(requirements & MAIN_JOB_ONLY)
//         then same requirement checks
//
// Out of scope: TRUST group, OnCanUseSpell override, blue IsSpellSet /
// UNBRIDLED_LEARNING membership, MOB/PET/TRUST branches.

namespace canusespelljobgateshelpers
{

// SPELLREQ bits from spell.h.
constexpr std::uint8_t SpellReqMerit             = 0x01;
constexpr std::uint8_t SpellReqAddendumBlack     = 0x02;
constexpr std::uint8_t SpellReqAddendumWhite     = 0x04;
constexpr std::uint8_t SpellReqTabulaRasa        = 0x08;
constexpr std::uint8_t SpellReqUnbridledLearning = 0x10;
constexpr std::uint8_t SpellReqMainJobOnly       = 0x20;

// JobLevelMeetsSpell: entityLevel >= spellJobLevel (getJob-mapped).
// Distinct from CanUseSpellWith (strict >).
constexpr auto JobLevelMeetsSpell(const std::uint8_t entityLevel, const std::uint8_t spellJobLevel) -> bool
{
    return entityLevel >= spellJobLevel;
}

// SubJobAllowed: !(requirements & MAIN_JOB_ONLY).
constexpr auto SubJobAllowed(const std::uint8_t requirements) -> bool
{
    return (requirements & SpellReqMainJobOnly) == 0;
}

// ApplyTabulaRasaGate: TABULA_RASA bit requires hasTabulaRasa.
constexpr auto ApplyTabulaRasaGate(const std::uint8_t requirements, const bool hasTabulaRasa) -> bool
{
    if ((requirements & SpellReqTabulaRasa) != 0)
    {
        return hasTabulaRasa;
    }
    return true;
}

// ApplySchAddendumGate: SCH-only addendum black/white with Enlightenment.
// Black takes priority when both bits are set (else-if in LSB).
constexpr auto ApplySchAddendumGate(
    const bool         isSCH,
    const std::uint8_t requirements,
    const bool         hasAddendumBlack,
    const bool         hasAddendumWhite,
    const bool         hasEnlightenment) -> bool
{
    if (!isSCH)
    {
        return true;
    }
    if ((requirements & SpellReqAddendumBlack) != 0)
    {
        return hasAddendumBlack || hasEnlightenment;
    }
    if ((requirements & SpellReqAddendumWhite) != 0)
    {
        return hasAddendumWhite || hasEnlightenment;
    }
    return true;
}

// ApplyRequirementGates: Tabula Rasa + SCH addendum after job-level pass.
// Blue UNBRIDLED_LEARNING / IsSpellSet omitted.
constexpr auto ApplyRequirementGates(
    const std::uint8_t requirements,
    const bool         isSCH,
    const bool         hasTabulaRasa,
    const bool         hasAddendumBlack,
    const bool         hasAddendumWhite,
    const bool         hasEnlightenment) -> bool
{
    bool usable = true;
    if (!ApplyTabulaRasaGate(requirements, hasTabulaRasa))
    {
        usable = false;
    }
    if (!ApplySchAddendumGate(isSCH, requirements, hasAddendumBlack, hasAddendumWhite, hasEnlightenment))
    {
        usable = false;
    }
    return usable;
}

// Injected params for the PC/Fellow/NPC job branch of CanUseSpell.
struct PCSpellJobParams
{
    std::uint8_t mainLevel{};
    std::uint8_t mainSpellJobLevel{};
    bool         mainIsSCH{};
    std::uint8_t subLevel{};
    std::uint8_t subSpellJobLevel{};
    bool         subIsSCH{};
    std::uint8_t requirements{};
    bool         hasTabulaRasa{};
    bool         hasAddendumBlack{};
    bool         hasAddendumWhite{};
    bool         hasEnlightenment{};
};

// ResolvePCSpellJobUsable: main path OR sub path after injects.
constexpr auto ResolvePCSpellJobUsable(const PCSpellJobParams& p) -> bool
{
    if (JobLevelMeetsSpell(p.mainLevel, p.mainSpellJobLevel))
    {
        if (ApplyRequirementGates(
                p.requirements,
                p.mainIsSCH,
                p.hasTabulaRasa,
                p.hasAddendumBlack,
                p.hasAddendumWhite,
                p.hasEnlightenment))
        {
            return true;
        }
    }
    if (JobLevelMeetsSpell(p.subLevel, p.subSpellJobLevel) && SubJobAllowed(p.requirements))
    {
        return ApplyRequirementGates(
            p.requirements,
            p.subIsSCH,
            p.hasTabulaRasa,
            p.hasAddendumBlack,
            p.hasAddendumWhite,
            p.hasEnlightenment);
    }
    return false;
}

} // namespace canusespelljobgateshelpers
