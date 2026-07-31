#include "test_skillchain_tables_1555.h"

#include "map/skillchain_tables_capacity.h"

#include <iostream>

namespace
{
using skillchaintableshelpers::GetSkillchainSubeffect;
using skillchaintableshelpers::GetSkillchainTier;
using skillchaintableshelpers::IsValidSkillchainElement;

auto Check() -> bool
{
    // Bounds
    if (!IsValidSkillchainElement(0) || !IsValidSkillchainElement(16) || IsValidSkillchainElement(17) || IsValidSkillchainElement(255))
    {
        return false;
    }

    // Tiers: None=0, Lv1=1, Lv2=2, Lv3=3, Lv4=4
    if (GetSkillchainTier(0) != 0)
    {
        return false;
    }
    // Transfixion..Impaction = 1..8 → tier 1
    for (std::uint8_t sc = 1; sc <= 8; ++sc)
    {
        if (GetSkillchainTier(sc) != 1)
        {
            return false;
        }
    }
    // Gravitation..Fragmentation = 9..12 → tier 2
    for (std::uint8_t sc = 9; sc <= 12; ++sc)
    {
        if (GetSkillchainTier(sc) != 2)
        {
            return false;
        }
    }
    if (GetSkillchainTier(13) != 3 || GetSkillchainTier(14) != 3)
    {
        return false;
    }
    if (GetSkillchainTier(15) != 4 || GetSkillchainTier(16) != 4)
    {
        return false;
    }
    if (GetSkillchainTier(17) != 0 || GetSkillchainTier(255) != 0)
    {
        return false;
    }

    // Subeffects: samples + Light_II / Darkness_II collapse
    if (GetSkillchainSubeffect(0) != ActionProcSkillChain::None)
    {
        return false;
    }
    if (GetSkillchainSubeffect(1) != ActionProcSkillChain::Transfixion)
    {
        return false;
    }
    if (GetSkillchainSubeffect(3) != ActionProcSkillChain::Liquefaction)
    {
        return false;
    }
    if (GetSkillchainSubeffect(9) != ActionProcSkillChain::Gravitation)
    {
        return false;
    }
    if (GetSkillchainSubeffect(13) != ActionProcSkillChain::Light)
    {
        return false;
    }
    if (GetSkillchainSubeffect(14) != ActionProcSkillChain::Darkness)
    {
        return false;
    }
    if (GetSkillchainSubeffect(15) != ActionProcSkillChain::Light)
    {
        return false;
    }
    if (GetSkillchainSubeffect(16) != ActionProcSkillChain::Darkness)
    {
        return false;
    }
    if (GetSkillchainSubeffect(17) != ActionProcSkillChain::None)
    {
        return false;
    }

    return true;
}
} // namespace

auto runSkillchainTables1555SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "skillchain_tables_1555 self-tests failed\n";
        return false;
    }
    return true;
}
