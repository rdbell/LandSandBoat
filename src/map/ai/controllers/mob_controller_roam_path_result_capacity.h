#pragma once

namespace mobcontrollerroampathresult
{
enum class Result
{
    RecordAction,
    Follow,
    Conceal,
};

inline auto Resolve(const bool pathCreated, const bool stealth) -> Result
{
    if (!pathCreated)
    {
        return Result::RecordAction;
    }

    return stealth ? Result::Conceal : Result::Follow;
}
} // namespace mobcontrollerroampathresult
