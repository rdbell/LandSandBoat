#pragma once

namespace trustvalidtargethelpers
{

struct Context
{
    bool passiveTrust{};
    bool initiatorIsTrust{};
    bool sameMaster{};
    bool pianissimoTarget{};
    bool entrustTarget{};
    bool playerPartyTarget{};
    bool sameAllegiance{};
    bool hasMaster{};
    bool initiatorIsSelf{};
    bool initiatorIsPet{};
    bool sameParty{};
};

template <typename HasPianissimo, typename BaseValidTarget>
inline auto Apply(const Context& ctx, HasPianissimo&& hasPianissimo, BaseValidTarget&& baseValidTarget) -> bool
{
    if (ctx.passiveTrust)
    {
        return false;
    }
    if (ctx.initiatorIsTrust && ctx.sameMaster)
    {
        return true;
    }
    if (ctx.pianissimoTarget && ctx.sameAllegiance && ctx.hasMaster && !ctx.initiatorIsSelf && hasPianissimo())
    {
        return true;
    }
    if (ctx.entrustTarget && ctx.sameAllegiance && ctx.hasMaster && !ctx.initiatorIsSelf)
    {
        return true;
    }
    if (ctx.playerPartyTarget && ctx.initiatorIsPet && ctx.sameAllegiance)
    {
        return true;
    }
    if (ctx.playerPartyTarget && ctx.sameAllegiance && ctx.hasMaster)
    {
        return ctx.sameParty;
    }
    return baseValidTarget();
}

} // namespace trustvalidtargethelpers
