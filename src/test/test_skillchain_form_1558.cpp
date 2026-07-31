#include "test_skillchain_form_1558.h"

#include "map/skillchain_tables_capacity.h"

#include <iostream>
#include <list>

namespace
{
using namespace skillchaintableshelpers;

auto Check() -> bool
{
    // FormSkillchain samples: (skill, resonance) → result
    // Level 3: Light + Light → Light II
    {
        const std::list<std::uint8_t> res{ SCElementLight };
        const std::list<std::uint8_t> sk{ SCElementLight };
        if (FormSkillchain(res, sk) != SCElementLightII)
        {
            return false;
        }
    }
    // Distortion + Gravitation (skill, resonance) → Darkness
    {
        const std::list<std::uint8_t> res{ SCElementGravitation };
        const std::list<std::uint8_t> sk{ SCElementDistortion };
        if (FormSkillchain(res, sk) != SCElementDarkness)
        {
            return false;
        }
    }
    // Scission + Transfixion → Distortion
    {
        const std::list<std::uint8_t> res{ SCElementTransfixion };
        const std::list<std::uint8_t> sk{ SCElementScission };
        if (FormSkillchain(res, sk) != SCElementDistortion)
        {
            return false;
        }
    }
    // First match: multiple resonance; first resonance wins when both match
    {
        const std::list<std::uint8_t> res{ SCElementTransfixion, SCElementCompression };
        const std::list<std::uint8_t> sk{ SCElementCompression };
        // Transfixion resonance + Compression skill → Compression
        if (FormSkillchain(res, sk) != SCElementCompression)
        {
            return false;
        }
    }
    // No match
    {
        const std::list<std::uint8_t> res{ SCElementLight };
        const std::list<std::uint8_t> sk{ SCElementScission };
        if (FormSkillchain(res, sk) != SCElementNone)
        {
            return false;
        }
    }
    // Lookup pair direct
    if (LookupSkillchainPair(SCElementLight, SCElementLight) != SCElementLightII)
    {
        return false;
    }
    if (LookupSkillchainPair(SCElementScission, SCElementLight) != SCElementNone)
    {
        return false;
    }
    if (LookupSkillchainPair(255, 255) != SCElementNone)
    {
        return false;
    }

    // Magic elements
    {
        const auto e = GetSkillchainMagicElement(SCElementTransfixion);
        if (e.size() != 1 || e[0] != ElementLight)
        {
            return false;
        }
    }
    {
        const auto e = GetSkillchainMagicElement(SCElementGravitation);
        if (e.size() != 2 || e[0] != ElementDark || e[1] != ElementEarth)
        {
            return false;
        }
    }
    {
        const auto e = GetSkillchainMagicElement(SCElementLightII);
        if (e.size() != 4 || e[0] != ElementLight || e[3] != ElementThunder)
        {
            return false;
        }
    }
    {
        const auto e = GetSkillchainMagicElement(SCElementNone);
        if (!e.empty())
        {
            return false;
        }
    }

    // Resistance rank mods
    {
        const auto m = GetResistanceRankModFromElement(ElementFire);
        if (!m || *m != Mod::FIRE_RES_RANK)
        {
            return false;
        }
    }
    {
        const auto m = GetResistanceRankModFromElement(ElementDark);
        if (!m || *m != Mod::DARK_RES_RANK)
        {
            return false;
        }
    }
    if (GetResistanceRankModFromElement(ElementNone).has_value())
    {
        return false;
    }

    return true;
}
} // namespace

auto runSkillchainForm1558SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "skillchain_form_1558 self-tests failed\n";
        return false;
    }
    return true;
}
