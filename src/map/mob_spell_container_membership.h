#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <ranges>
#include <vector>

// Pure CMobSpellContainer AddSpell/RemoveSpell classification seam. Catalog
// lookup supplies spell_membership_info; entity, recast, and RNG hosts stay out.
enum class mob_spell_category : uint8
{
    GA,
    Damage,
    Buff,
    Debuff,
    Heal,
    Na,
    Raise,
    Severe,
};

struct spell_membership_info
{
    bool aoe;
    bool targetsEnemy;
    bool severe;
    bool debuff;
    bool na;
    bool raise;
    bool heal;
    bool buff;
};

inline auto ClassifyMobSpellMembership(const spell_membership_info& info, mob_spell_category& category) -> bool
{
    if (info.aoe && info.targetsEnemy)
    {
        category = mob_spell_category::GA;
        return true;
    }
    if (info.severe)
    {
        category = mob_spell_category::Severe;
        return true;
    }
    if (info.targetsEnemy)
    {
        category = mob_spell_category::Damage;
        return true;
    }
    if (info.debuff)
    {
        category = mob_spell_category::Debuff;
        return true;
    }
    if (info.na)
    {
        category = mob_spell_category::Na;
        return true;
    }
    if (info.raise)
    {
        category = mob_spell_category::Raise;
        return true;
    }
    if (info.heal)
    {
        category = mob_spell_category::Heal;
        return true;
    }
    if (info.buff)
    {
        category = mob_spell_category::Buff;
        return true;
    }
    return false;
}

class mob_spell_membership
{
public:
    auto add(const uint16 id, const spell_membership_info& info, const bool catalogFound) -> bool
    {
        mob_spell_category category{};
        if (!catalogFound)
        {
            return false;
        }
        hasSpells = true;
        if (!ClassifyMobSpellMembership(info, category))
        {
            return false;
        }
        lists[static_cast<size_t>(category)].push_back(id);
        return true;
    }

    void remove(const uint16 id)
    {
        // LSB omission: Severe is not removed and does not count here.
        for (size_t i = 0; i < static_cast<size_t>(mob_spell_category::Severe); ++i)
        {
            auto& list = lists[i];
            list.erase(std::remove(list.begin(), list.end(), id), list.end());
        }
        hasSpells = std::ranges::any_of(lists.begin(), lists.begin() + static_cast<size_t>(mob_spell_category::Severe), [](const auto& list)
                                        {
                                            return !list.empty();
                                        });
    }

    void clear()
    {
        lists     = {};
        hasSpells = false;
    }

    auto has() const -> bool
    {
        return hasSpells;
    }

    auto list(const mob_spell_category category) const -> const std::vector<uint16>&
    {
        return lists[static_cast<size_t>(category)];
    }

private:
    std::array<std::vector<uint16>, 8> lists{};
    bool                               hasSpells = false;
};

template <typename ID>
inline auto RemoveMobSpellMembership(const ID         id,
                                     std::vector<ID>& ga,
                                     std::vector<ID>& damage,
                                     std::vector<ID>& buff,
                                     std::vector<ID>& debuff,
                                     std::vector<ID>& heal,
                                     std::vector<ID>& na,
                                     std::vector<ID>& raise) -> bool
{
    auto remove = [id](auto& list)
    {
        list.erase(std::remove(list.begin(), list.end(), id), list.end());
    };
    remove(ga);
    remove(damage);
    remove(buff);
    remove(debuff);
    remove(heal);
    remove(na);
    remove(raise);
    return !(ga.empty() && damage.empty() && buff.empty() && debuff.empty() && heal.empty() && na.empty() && raise.empty());
}
