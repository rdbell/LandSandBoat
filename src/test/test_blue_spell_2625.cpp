#include "test_blue_spell_2625.h"

#include <iostream>

#include "map/blue_spell.h"

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "blue spell self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runBlueSpell2625SelfTests() -> bool
{
    auto spell = CBlueSpell{ static_cast<SpellID>(577) };
    bool ok    = true;
    ok         = expect(spell.getMonsterSkillId() == 0 && spell.getSetPoints() == 0 && spell.getEcosystem() == 0 && spell.getTraitCategory() == 0 && spell.getTraitWeight() == 0, "defaults") && ok;
    ok         = expect(spell.getPrimarySkillchain() == 0 && spell.getSecondarySkillchain() == 0 && spell.getTertiarySkillchain() == 0 && spell.getKnockback() == Knockback::None && spell.modList.empty(), "remaining defaults") && ok;
    spell.setMonsterSkillId(1234);
    spell.setSetPoints(3);
    spell.setEcosystem(7);
    spell.setTraitCategory(2);
    spell.setTraitWeight(5);
    spell.setPrimarySkillchain(1);
    spell.setSecondarySkillchain(4);
    spell.setTertiarySkillchain(8);
    spell.setKnockback(Knockback::Level3);
    spell.addModifier(CModifier{ Mod::HP, 20 });
    spell.addModifier(CModifier{ Mod::MP, -5 });
    ok                = expect(spell.getMonsterSkillId() == 1234 && spell.getSetPoints() == 3 && spell.getEcosystem() == 7 && spell.getTraitCategory() == 2 && spell.getTraitWeight() == 5, "setters") && ok;
    ok                = expect(spell.getPrimarySkillchain() == 1 && spell.getSecondarySkillchain() == 4 && spell.getTertiarySkillchain() == 8 && spell.getKnockback() == Knockback::Level3, "skillchains and knockback") && ok;
    ok                = expect(spell.modList.size() == 2 && spell.modList[0].getModID() == Mod::HP && spell.modList[0].getModAmount() == 20 && spell.modList[1].getModID() == Mod::MP && spell.modList[1].getModAmount() == -5, "modifier order") && ok;
    const auto  clone = spell.clone();
    const auto* blue  = dynamic_cast<const CBlueSpell*>(clone.get());
    return expect(blue != nullptr && blue->modList.size() == 2 && blue->getMonsterSkillId() == 1234, "clone copies extension state") && ok;
}
