#pragma once

namespace trustcastfinishedhelpers
{

template <typename BaseCastFinished, typename SpellRecastID, typename AddMagicRecast>
inline void Apply(BaseCastFinished&& baseCastFinished, SpellRecastID&& spellRecastID, AddMagicRecast&& addMagicRecast)
{
    baseCastFinished();
    const auto recastID = spellRecastID();
    addMagicRecast(recastID);
}

} // namespace trustcastfinishedhelpers
