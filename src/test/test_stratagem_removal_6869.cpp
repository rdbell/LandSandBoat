#include "test_stratagem_removal_6869.h"

#include "map/char_stratagem_removal.h"

#include <cstddef>
#include <initializer_list>
#include <iostream>

auto runStratagemRemoval6869SelfTests() -> bool
{
    const auto matches = [](const stratagemremovalhelpers::Plan& plan, const std::initializer_list<stratagemremovalhelpers::Effect> effects) {
        if (static_cast<std::size_t>(plan.count) != effects.size())
        {
            return false;
        }

        std::uint8_t index{};
        for (const auto effect : effects)
        {
            if (plan.effects[index++] != effect)
            {
                return false;
            }
        }
        return true;
    };

    bool ok = true;
    const auto other = stratagemremovalhelpers::MakePlan(stratagemremovalhelpers::SpellGroup::Other, false, false, false);
    ok               = matches(other, {}) && ok;
    const auto whiteBase = stratagemremovalhelpers::MakePlan(stratagemremovalhelpers::SpellGroup::White, false, false, false);
    ok                  = matches(whiteBase, {
                             stratagemremovalhelpers::Effect::Penury,
                             stratagemremovalhelpers::Effect::Celerity,
                             stratagemremovalhelpers::Effect::Enlightenment,
                             stratagemremovalhelpers::Effect::Altruism,
                             stratagemremovalhelpers::Effect::Tranquility,
                         }) &&
        ok;
    const auto white = stratagemremovalhelpers::MakePlan(stratagemremovalhelpers::SpellGroup::White, true, false, true);
    ok               = matches(white, {
                         stratagemremovalhelpers::Effect::Penury,
                         stratagemremovalhelpers::Effect::Celerity,
                         stratagemremovalhelpers::Effect::Enlightenment,
                         stratagemremovalhelpers::Effect::Altruism,
                         stratagemremovalhelpers::Effect::Tranquility,
                         stratagemremovalhelpers::Effect::Accession,
                         stratagemremovalhelpers::Effect::Perpetuance,
                     }) &&
        ok;
    const auto blackBase = stratagemremovalhelpers::MakePlan(stratagemremovalhelpers::SpellGroup::Black, false, false, false);
    ok                  = matches(blackBase, {
                             stratagemremovalhelpers::Effect::Parsimony,
                             stratagemremovalhelpers::Effect::Alacrity,
                             stratagemremovalhelpers::Effect::Enlightenment,
                             stratagemremovalhelpers::Effect::Focalization,
                             stratagemremovalhelpers::Effect::Equanimity,
                         }) &&
        ok;
    const auto black = stratagemremovalhelpers::MakePlan(stratagemremovalhelpers::SpellGroup::Black, false, true, false);
    ok               = matches(black, {
                         stratagemremovalhelpers::Effect::Parsimony,
                         stratagemremovalhelpers::Effect::Alacrity,
                         stratagemremovalhelpers::Effect::Enlightenment,
                         stratagemremovalhelpers::Effect::Focalization,
                         stratagemremovalhelpers::Effect::Equanimity,
                         stratagemremovalhelpers::Effect::Manifestation,
                     }) &&
        ok;
    if (!ok)
    {
        std::cerr << "stratagem removal 6869 self-test failed\n";
    }
    return ok;
}
