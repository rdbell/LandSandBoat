#pragma once

#include <functional>
#include <utility>

namespace charvalidtargethelpers
{

struct Context
{
    bool confrontationMatches{};
    bool dead{};
    bool targetPlayerDead{};
    bool targetPlayer{};
    bool sameAllegiance{};
};

struct Relations
{
    bool sameParty{};
    bool sameAlliance{};
    bool partyPetMaster{};
    bool soloPetMaster{};
    bool targetsParty{};
    bool targetsAlliance{};
    bool pianissimoTarget{};
    bool entrustTarget{};
    bool differentCharacter{};
    bool initiatorIsTrust{};
};

template <typename BaseValidTarget, typename LoadRelations, typename HasPianissimo, typename HasEntrust>
inline auto Apply(
    const Context&    ctx,
    BaseValidTarget&& baseValidTarget,
    LoadRelations&&   loadRelations,
    HasPianissimo&&   hasPianissimo,
    HasEntrust&&      hasEntrust) -> bool
{
    if (!ctx.confrontationMatches)
    {
        return false;
    }

    if (ctx.dead)
    {
        return ctx.targetPlayerDead;
    }

    if (ctx.targetPlayer && ctx.sameAllegiance)
    {
        return true;
    }

    if (std::invoke(std::forward<BaseValidTarget>(baseValidTarget)))
    {
        return true;
    }

    const auto relations = std::invoke(std::forward<LoadRelations>(loadRelations));
    const bool pianissimo = relations.pianissimoTarget &&
                           std::invoke(std::forward<HasPianissimo>(hasPianissimo));
    const bool entrust = relations.entrustTarget &&
                         std::invoke(std::forward<HasEntrust>(hasEntrust));

    if (relations.targetsAlliance && relations.sameAlliance && relations.differentCharacter)
    {
        return true;
    }

    if ((relations.targetsParty || pianissimo) &&
        (relations.sameParty || relations.partyPetMaster || relations.soloPetMaster) &&
        relations.differentCharacter)
    {
        return true;
    }

    return entrust && (relations.sameParty || relations.initiatorIsTrust);
}

} // namespace charvalidtargethelpers
