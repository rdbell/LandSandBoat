#pragma once

namespace petcontrollerhealing
{

struct Plan
{
    bool start;
    bool stop;
    bool isHealing;
};

inline auto Resolve(bool masterHealing, bool petHealing, bool preventAction) -> Plan
{
    if (masterHealing && !petHealing && !preventAction)
    {
        return { true, false, true };
    }
    if (!masterHealing && petHealing)
    {
        return { false, true, false };
    }
    return { false, false, masterHealing };
}

} // namespace petcontrollerhealing
