/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_login_display_bitmasks.h"

#include "common/settings.h"
#include "login/login_helpers.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{

using SettingBit = std::pair<std::string, uint16>;
using SettingValue = std::pair<std::string, bool>;

const std::vector<SettingBit> expansionSettings{
    { "login.RISE_OF_ZILART", loginHelpers::EXPANSION_DISPLAY::RISE_OF_ZILART },
    { "login.CHAINS_OF_PROMATHIA", loginHelpers::EXPANSION_DISPLAY::CHAINS_OF_PROMATHIA },
    { "login.TREASURES_OF_AHT_URGHAN", loginHelpers::EXPANSION_DISPLAY::TREASURES_OF_AHT_URGHAN },
    { "login.WINGS_OF_THE_GODDESS", loginHelpers::EXPANSION_DISPLAY::WINGS_OF_THE_GODDESS },
    { "login.A_CRYSTALLINE_PROPHECY", loginHelpers::EXPANSION_DISPLAY::A_CRYSTALLINE_PROPHECY },
    { "login.A_MOOGLE_KUPOD_ETAT", loginHelpers::EXPANSION_DISPLAY::A_MOOGLE_KUPOD_ETAT },
    { "login.A_SHANTOTTO_ASCENSION", loginHelpers::EXPANSION_DISPLAY::A_SHANTOTTO_ASCENSION },
    { "login.VISIONS_OF_ABYSSEA", loginHelpers::EXPANSION_DISPLAY::VISIONS_OF_ABYSSEA },
    { "login.SCARS_OF_ABYSSEA", loginHelpers::EXPANSION_DISPLAY::SCARS_OF_ABYSSEA },
    { "login.HEROES_OF_ABYSSEA", loginHelpers::EXPANSION_DISPLAY::HEROES_OF_ABYSSEA },
    { "login.SEEKERS_OF_ADOULIN", loginHelpers::EXPANSION_DISPLAY::SEEKERS_OF_ADOULIN },
};

const std::vector<SettingBit> featureSettings{
    { "login.SECURE_TOKEN", loginHelpers::FEATURE_DISPLAY::SECURE_TOKEN },
    { "login.MOG_WARDROBE_3", loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_3 },
    { "login.MOG_WARDROBE_4", loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_4 },
    { "login.MOG_WARDROBE_5", loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_5 },
    { "login.MOG_WARDROBE_6", loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_6 },
    { "login.MOG_WARDROBE_7", loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_7 },
    { "login.MOG_WARDROBE_8", loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_8 },
};

auto expectEqual(uint16 actual, uint16 expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login display bitmasks self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto resetSettings(const std::vector<SettingBit>& settings) -> void
{
    for (const auto& [key, _] : settings)
    {
        settings::set(key, false);
    }
}

auto saveSettings(const std::vector<SettingBit>& settings) -> std::vector<SettingValue>
{
    auto saved = std::vector<SettingValue>{};
    saved.reserve(settings.size());
    for (const auto& [key, _] : settings)
    {
        saved.emplace_back(key, settings::get<bool>(key));
    }
    return saved;
}

auto restoreSettings(const std::vector<SettingValue>& saved) -> void
{
    for (const auto& [key, value] : saved)
    {
        settings::set(key, value);
    }
}

auto setAll(const std::vector<SettingBit>& settings) -> uint16
{
    uint16 mask = 0;
    for (const auto& [key, bit] : settings)
    {
        settings::set(key, true);
        mask |= bit;
    }
    return mask;
}

auto testExpansionBitmask() -> bool
{
    bool ok = true;

    resetSettings(expansionSettings);
    ok = expectEqual(loginHelpers::generateExpansionBitmask(), loginHelpers::EXPANSION_DISPLAY::BASE_GAME, "base only") && ok;

    settings::set("login.RISE_OF_ZILART", true);
    settings::set("login.WINGS_OF_THE_GODDESS", true);
    settings::set("login.SEEKERS_OF_ADOULIN", true);
    ok = expectEqual(
             loginHelpers::generateExpansionBitmask(),
             loginHelpers::EXPANSION_DISPLAY::BASE_GAME |
                 loginHelpers::EXPANSION_DISPLAY::RISE_OF_ZILART |
                 loginHelpers::EXPANSION_DISPLAY::WINGS_OF_THE_GODDESS |
                 loginHelpers::EXPANSION_DISPLAY::SEEKERS_OF_ADOULIN,
             "selected expansions") &&
         ok;

    resetSettings(expansionSettings);
    const auto allExpansions = setAll(expansionSettings);
    ok = expectEqual(
             loginHelpers::generateExpansionBitmask(),
             static_cast<uint16>(loginHelpers::EXPANSION_DISPLAY::BASE_GAME | allExpansions),
             "all expansions") &&
         ok;

    resetSettings(expansionSettings);
    return ok;
}

auto testFeatureBitmask() -> bool
{
    bool ok = true;

    resetSettings(featureSettings);
    ok = expectEqual(loginHelpers::generateFeatureBitmask(), 0, "no features") && ok;

    settings::set("login.SECURE_TOKEN", true);
    settings::set("login.MOG_WARDROBE_4", true);
    settings::set("login.MOG_WARDROBE_8", true);
    ok = expectEqual(
             loginHelpers::generateFeatureBitmask(),
             loginHelpers::FEATURE_DISPLAY::SECURE_TOKEN |
                 loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_4 |
                 loginHelpers::FEATURE_DISPLAY::MOG_WARDROBE_8,
             "selected features") &&
         ok;

    resetSettings(featureSettings);
    ok = expectEqual(loginHelpers::generateFeatureBitmask(), setAll(featureSettings), "all features") && ok;

    resetSettings(featureSettings);
    return ok;
}

} // namespace

auto runLoginDisplayBitmasksSelfTests() -> bool
{
    bool ok = true;
    const auto savedExpansions = saveSettings(expansionSettings);
    const auto savedFeatures = saveSettings(featureSettings);

    ok = testExpansionBitmask() && ok;
    ok = testFeatureBitmask() && ok;

    restoreSettings(savedExpansions);
    restoreSettings(savedFeatures);

    return ok;
}
