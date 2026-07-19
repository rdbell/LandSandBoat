#pragma once

namespace mobcontrollerspelltargetsource
{
enum class Source
{
    Enemy,
    Self,
    Master,
    PartyCandidate,
};

// Select mirrors CastSpell's target-source priority before party lookup.
constexpr auto Select(const bool selfTarget, const bool partyTarget, const bool hasMaster, const bool chooseMaster, const bool chooseParty) -> Source
{
    if (!selfTarget)
    {
        return Source::Enemy;
    }
    if (!partyTarget)
    {
        return Source::Self;
    }
    if (hasMaster && chooseMaster)
    {
        return Source::Master;
    }
    return chooseParty ? Source::PartyCandidate : Source::Self;
}
} // namespace mobcontrollerspelltargetsource
