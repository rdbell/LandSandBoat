#include "test_automaton_skill_cap_1588.h"

#include "map/automaton_skill_cap_capacity.h"

#include <iostream>

namespace
{
using namespace automatonskillcaphelpers;

auto Check() -> bool
{
    if (!IsAutomatonSkill(SkillMelee) || !IsAutomatonSkill(SkillMagic) || IsAutomatonSkill(21) || IsAutomatonSkill(25))
    {
        return false;
    }
    if (CapLevel(50) != 50 || CapLevel(99) != 99 || CapLevel(100) != 99)
    {
        return false;
    }
    if (!IsKnownFrame(FrameHarlequin) || IsKnownFrame(0))
    {
        return false;
    }

    if (FrameSkillRank(FrameHarlequin, SkillMelee) != RankBMinus ||
        FrameSkillRank(FrameValoredge, SkillRanged) != 0 ||
        FrameHasSkill(FrameSharpshot, SkillRanged) == false ||
        FrameHasSkill(FrameStormwaker, SkillRanged) == true)
    {
        return false;
    }
    if (HeadSkillBonus(HeadValoredge, SkillMelee) != HeadBonus ||
        HeadSkillBonus(HeadValoredge, SkillRanged) != 0 ||
        HeadSkillBonus(HeadSoulsoother, SkillMagic) != HeadBonus ||
        HeadSkillBonus(HeadHarlequin, SkillMagic) != 0)
    {
        return false;
    }

    // Harlequin melee B-
    if (SkillCapRank(FrameHarlequin, HeadHarlequin, SkillMelee) != RankBMinus)
    {
        return false;
    }
    // Valoredge+Valoredge melee A+
    if (SkillCapRank(FrameValoredge, HeadValoredge, SkillMelee) != RankAPlus)
    {
        return false;
    }
    // Valoredge + Stormwaker head magic → F via 13 + (-2)
    if (SkillCapRank(FrameValoredge, HeadStormwaker, SkillMagic) != RankF)
    {
        return false;
    }
    // Neither grants ranged
    if (SkillCapRank(FrameValoredge, HeadHarlequin, SkillRanged) != 0)
    {
        return false;
    }
    // Sharpshot+Sharpshot ranged A+
    if (SkillCapRank(FrameSharpshot, HeadSharpshot, SkillRanged) != RankAPlus)
    {
        return false;
    }
    // Stormwaker+Soulsoother magic A+
    if (SkillCapRank(FrameStormwaker, HeadSoulsoother, SkillMagic) != RankAPlus)
    {
        return false;
    }
    // Stormwaker+Spiritreaver magic A+
    if (SkillCapRank(FrameStormwaker, HeadSpiritreaver, SkillMagic) != RankAPlus)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAutomatonSkillCap1588SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "automaton_skill_cap_1588 self-tests failed\n";
        return false;
    }
    return true;
}
