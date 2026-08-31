#include "test_spell_four_cc_8640.h"

#include "map/enums/four_cc.h"
#include "map/spell.h"
#include "omega_self_test_registry.h"

#include <iostream>
#include <utility>

namespace
{
using Entry = std::pair<SPELLGROUP, std::pair<FourCC, FourCC>>;

auto Check() -> bool
{
    const Entry cases[] = {
        { SPELLGROUP_WHITE, { FourCC::WhiteMagicCast, FourCC::WhiteMagicInterrupt } },
        { SPELLGROUP_BLACK, { FourCC::BlackMagicCast, FourCC::BlackMagicInterrupt } },
        { SPELLGROUP_BLUE, { FourCC::BlueMagicCast, FourCC::BlueMagicInterrupt } },
        { SPELLGROUP_SONG, { FourCC::SongMagicCast, FourCC::SongMagicInterrupt } },
        { SPELLGROUP_NINJUTSU, { FourCC::NinjutsuMagicCast, FourCC::NinjutsuMagicInterrupt } },
        { SPELLGROUP_SUMMONING, { FourCC::SummonMagicCast, FourCC::SummonMagicInterrupt } },
        { SPELLGROUP_GEOMANCY, { FourCC::GeomancyMagicCast, FourCC::GeomancyMagicInterrupt } },
        { SPELLGROUP_TRUST, { FourCC::TrustMagicCast, FourCC::TrustMagicInterrupt } },
    };

    for (const auto& [group, expected] : cases)
    {
        CSpell spell(static_cast<SpellID>(1));
        spell.setSpellGroup(group);
        if (spell.getFourCC(false) != expected.first || spell.getFourCC(true) != expected.second)
        {
            return false;
        }
    }

    // NONE and unknown groups intentionally use the white interrupt fallback,
    // including a non-interrupt request.
    for (const auto group : { SPELLGROUP_NONE, static_cast<SPELLGROUP>(99) })
    {
        CSpell spell(static_cast<SpellID>(1));
        spell.setSpellGroup(group);
        if (spell.getFourCC(false) != FourCC::WhiteMagicInterrupt || spell.getFourCC(true) != FourCC::WhiteMagicInterrupt)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runSpellFourCC8640SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "spell_four_cc_8640 self-tests failed\n";
        return false;
    }
    return true;
}

OMEGA_REGISTER_SELF_TEST("spell-four-cc-8640", runSpellFourCC8640SelfTests);
