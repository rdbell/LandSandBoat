#include "test_can_use_spell_job_gates_1732.h"

#include "map/can_use_spell_job_gates_capacity.h"

#include <iostream>

namespace
{
using namespace canusespelljobgateshelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "can_use_spell_job_gates_1732 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runCanUseSpellJobGates1732SelfTests() -> bool
{
    bool ok = true;

    // --- SPELLREQ bit pins ---
    ok = expect(SpellReqMerit == 0x01, "SpellReqMerit pin") && ok;
    ok = expect(SpellReqAddendumBlack == 0x02, "SpellReqAddendumBlack pin") && ok;
    ok = expect(SpellReqAddendumWhite == 0x04, "SpellReqAddendumWhite pin") && ok;
    ok = expect(SpellReqTabulaRasa == 0x08, "SpellReqTabulaRasa pin") && ok;
    ok = expect(SpellReqUnbridledLearning == 0x10, "SpellReqUnbridledLearning pin") && ok;
    ok = expect(SpellReqMainJobOnly == 0x20, "SpellReqMainJobOnly pin") && ok;

    // --- JobLevelMeetsSpell (>=, not strict >) ---
    ok = expect(JobLevelMeetsSpell(50, 50), "equal level meets") && ok;
    ok = expect(JobLevelMeetsSpell(51, 50), "above meets") && ok;
    ok = expect(!JobLevelMeetsSpell(49, 50), "below fails") && ok;
    ok = expect(JobLevelMeetsSpell(0, 0), "zero vs zero") && ok;
    ok = expect(!JobLevelMeetsSpell(99, 255), "any vs unusable 255") && ok;
    ok = expect(JobLevelMeetsSpell(255, 255), "255 vs 255") && ok;

    // --- SubJobAllowed ---
    ok = expect(SubJobAllowed(0), "no flags allow sub") && ok;
    ok = expect(SubJobAllowed(SpellReqTabulaRasa | SpellReqAddendumBlack), "other bits allow sub") && ok;
    ok = expect(!SubJobAllowed(SpellReqMainJobOnly), "MAIN_JOB_ONLY blocks sub") && ok;
    ok = expect(!SubJobAllowed(SpellReqMainJobOnly | SpellReqTabulaRasa), "MAIN_JOB_ONLY with bits blocks sub") && ok;

    // --- ApplyTabulaRasaGate ---
    ok = expect(ApplyTabulaRasaGate(0, false), "no tabula req") && ok;
    ok = expect(ApplyTabulaRasaGate(SpellReqTabulaRasa, true), "tabula with effect") && ok;
    ok = expect(!ApplyTabulaRasaGate(SpellReqTabulaRasa, false), "tabula without effect") && ok;

    // --- ApplySchAddendumGate ---
    ok = expect(ApplySchAddendumGate(false, SpellReqAddendumBlack, false, false, false), "non-SCH ignores black") && ok;
    ok = expect(ApplySchAddendumGate(false, SpellReqAddendumWhite, false, false, false), "non-SCH ignores white") && ok;
    ok = expect(ApplySchAddendumGate(true, 0, false, false, false), "SCH no addendum req") && ok;
    ok = expect(!ApplySchAddendumGate(true, SpellReqAddendumBlack, false, false, false), "SCH black without") && ok;
    ok = expect(ApplySchAddendumGate(true, SpellReqAddendumBlack, true, false, false), "SCH black with black") && ok;
    ok = expect(ApplySchAddendumGate(true, SpellReqAddendumBlack, false, false, true), "SCH black with enlightenment") && ok;
    ok = expect(!ApplySchAddendumGate(true, SpellReqAddendumWhite, false, false, false), "SCH white without") && ok;
    ok = expect(ApplySchAddendumGate(true, SpellReqAddendumWhite, false, true, false), "SCH white with white") && ok;
    ok = expect(ApplySchAddendumGate(true, SpellReqAddendumWhite, false, false, true), "SCH white with enlightenment") && ok;
    // Black priority when both bits set.
    ok = expect(!ApplySchAddendumGate(true, SpellReqAddendumBlack | SpellReqAddendumWhite, false, true, false),
                "both bits white-only fails") &&
         ok;
    ok = expect(ApplySchAddendumGate(true, SpellReqAddendumBlack | SpellReqAddendumWhite, true, false, false),
                "both bits black ok") &&
         ok;
    ok = expect(ApplySchAddendumGate(true, SpellReqAddendumBlack | SpellReqAddendumWhite, false, false, true),
                "both bits enlightenment ok") &&
         ok;

    // --- ApplyRequirementGates ---
    ok = expect(!ApplyRequirementGates(SpellReqTabulaRasa, false, false, false, false, false), "req tabula fail") && ok;
    ok = expect(!ApplyRequirementGates(SpellReqAddendumBlack, true, false, false, false, false), "req black fail") && ok;
    ok = expect(!ApplyRequirementGates(SpellReqTabulaRasa | SpellReqAddendumBlack, true, false, true, false, false),
                "tabula fail with black ok") &&
         ok;
    ok = expect(ApplyRequirementGates(SpellReqTabulaRasa | SpellReqAddendumWhite, true, true, false, true, false),
                "tabula+white with effects") &&
         ok;
    ok = expect(ApplyRequirementGates(SpellReqAddendumWhite, true, false, false, false, true),
                "enlightenment covers white") &&
         ok;

    // --- ResolvePCSpellJobUsable: main-only ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 50;
        p.mainSpellJobLevel = 40;
        p.subLevel          = 1;
        p.subSpellJobLevel  = 255;
        ok                  = expect(ResolvePCSpellJobUsable(p), "main-only usable") && ok;
        p.mainLevel         = 30;
        ok                  = expect(!ResolvePCSpellJobUsable(p), "main below level") && ok;
    }

    // --- sub-only ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 75;
        p.mainSpellJobLevel = 255;
        p.subLevel          = 30;
        p.subSpellJobLevel  = 25;
        ok                  = expect(ResolvePCSpellJobUsable(p), "sub-only usable") && ok;
        p.subLevel          = 20;
        ok                  = expect(!ResolvePCSpellJobUsable(p), "sub below level") && ok;
    }

    // --- MAIN_JOB_ONLY blocks sub ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 10;
        p.mainSpellJobLevel = 255;
        p.subLevel          = 50;
        p.subSpellJobLevel  = 30;
        p.requirements      = SpellReqMainJobOnly;
        ok                  = expect(!ResolvePCSpellJobUsable(p), "MAIN_JOB_ONLY blocks sub") && ok;
        p.mainSpellJobLevel = 5;
        ok                  = expect(ResolvePCSpellJobUsable(p), "MAIN_JOB_ONLY allows main") && ok;
    }

    // --- Tabula Rasa ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 99;
        p.mainSpellJobLevel = 1;
        p.subLevel          = 1;
        p.subSpellJobLevel  = 255;
        p.requirements      = SpellReqTabulaRasa;
        ok                  = expect(!ResolvePCSpellJobUsable(p), "tabula without effect") && ok;
        p.hasTabulaRasa     = true;
        ok                  = expect(ResolvePCSpellJobUsable(p), "tabula with effect") && ok;
    }

    // --- SCH addendum + Enlightenment ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 75;
        p.mainSpellJobLevel = 30;
        p.mainIsSCH         = true;
        p.subLevel          = 1;
        p.subSpellJobLevel  = 255;
        p.requirements      = SpellReqAddendumBlack;
        ok                  = expect(!ResolvePCSpellJobUsable(p), "SCH black without") && ok;
        p.hasAddendumBlack  = true;
        ok                  = expect(ResolvePCSpellJobUsable(p), "SCH black with black") && ok;
        p.hasAddendumBlack  = false;
        p.hasEnlightenment  = true;
        ok                  = expect(ResolvePCSpellJobUsable(p), "SCH black with enlightenment") && ok;

        p                   = PCSpellJobParams{};
        p.mainLevel         = 75;
        p.mainSpellJobLevel = 30;
        p.mainIsSCH         = true;
        p.subLevel          = 1;
        p.subSpellJobLevel  = 255;
        p.requirements      = SpellReqAddendumWhite;
        ok                  = expect(!ResolvePCSpellJobUsable(p), "SCH white without") && ok;
        p.hasAddendumWhite  = true;
        ok                  = expect(ResolvePCSpellJobUsable(p), "SCH white with white") && ok;
        p.hasAddendumWhite  = false;
        p.hasEnlightenment  = true;
        ok                  = expect(ResolvePCSpellJobUsable(p), "SCH white with enlightenment") && ok;
    }

    // --- non-SCH ignores addendum ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 75;
        p.mainSpellJobLevel = 30;
        p.mainIsSCH         = false;
        p.subLevel          = 1;
        p.subSpellJobLevel  = 255;
        p.requirements      = SpellReqAddendumBlack | SpellReqAddendumWhite;
        ok                  = expect(ResolvePCSpellJobUsable(p), "non-SCH ignores addendum") && ok;
    }

    // --- main SCH fails addendum, falls through to non-SCH sub ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 99;
        p.mainSpellJobLevel = 1;
        p.mainIsSCH         = true;
        p.subLevel          = 50;
        p.subSpellJobLevel  = 10;
        p.subIsSCH          = false;
        p.requirements      = SpellReqAddendumBlack;
        ok                  = expect(ResolvePCSpellJobUsable(p), "fall through to non-SCH sub") && ok;
    }

    // --- equal-level boundary (>=) ---
    {
        PCSpellJobParams p{};
        p.mainLevel         = 40;
        p.mainSpellJobLevel = 40;
        p.subLevel          = 0;
        p.subSpellJobLevel  = 255;
        ok                  = expect(ResolvePCSpellJobUsable(p), "equal level >= meets") && ok;
        p.mainLevel         = 39;
        ok                  = expect(!ResolvePCSpellJobUsable(p), "level-1 fails") && ok;
    }

    return ok;
}
