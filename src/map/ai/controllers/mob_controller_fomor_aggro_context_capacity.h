#pragma once

namespace mobcontrollerfomoraggrocontext
{
constexpr int DefaultHate = 8;

struct Context
{
    bool usesTargetHate;
    int  fallbackHate;
};

// Resolve determines whether a CoP Fomor must use a player's stored hate value.
constexpr auto Resolve(const bool fomor, const bool notorious, const bool zoneIsCoP, const bool targetIsPlayer) -> Context
{
    return { fomor && !notorious && zoneIsCoP && targetIsPlayer, DefaultHate };
}
} // namespace mobcontrollerfomoraggrocontext
