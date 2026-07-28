#pragma once

#include <array>

namespace instanceloader
{

enum class MobFinalizationAction
{
    Initialize,
    FindParty,
    ApplyMixins,
    SaveModifiers,
    SaveMobModifiers,
    CacheScript,
};

enum class NpcFinalizationAction
{
    Spawn,
    CacheScript,
};

enum class InstanceFinalizationAction
{
    CacheScript,
    CreatedCallback,
    Created,
};

inline constexpr std::array MobFinalizationOrder{
    MobFinalizationAction::Initialize,
    MobFinalizationAction::FindParty,
    MobFinalizationAction::ApplyMixins,
    MobFinalizationAction::SaveModifiers,
    MobFinalizationAction::SaveMobModifiers,
    MobFinalizationAction::CacheScript,
};

inline constexpr std::array NpcFinalizationOrder{
    NpcFinalizationAction::Spawn,
    NpcFinalizationAction::CacheScript,
};

inline constexpr std::array InstanceFinalizationOrder{
    InstanceFinalizationAction::CacheScript,
    InstanceFinalizationAction::CreatedCallback,
    InstanceFinalizationAction::Created,
};

} // namespace instanceloader
