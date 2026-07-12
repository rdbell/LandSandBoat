#pragma once

#include <cstdint>
#include <vector>

// Pure AddTraits decision and cure/in-range enmity filters.

namespace traitsenmityhelpers
{

struct TraitCandidate
{
    std::uint16_t id{ 0 };
    std::uint8_t  level{ 0 };
    std::uint8_t  rank{ 0 };
    std::uint16_t mod{ 0 };
    std::uint16_t meritID{ 0 };
};

struct ExistingTrait
{
    std::uint16_t id{ 0 };
    std::uint8_t  rank{ 0 };
    std::uint16_t mod{ 0 };
    std::uint16_t meritID{ 0 };
};

struct TraitDecision
{
    bool eligible{ false };
    bool shouldAdd{ false };
    int  delExistingIndex{ -1 };
};

template <typename MeritCountExisting, typename MeritCountCandidate>
inline auto ResolveTraitAdd(const std::uint8_t                   level,
                            const TraitCandidate&                candidate,
                            const std::vector<ExistingTrait>&    existing,
                            const bool                           isPC,
                            MeritCountExisting&&                 meritCountExisting,
                            MeritCountCandidate&&                meritCountCandidate) -> TraitDecision
{
    if (level < candidate.level || candidate.level == 0)
    {
        return TraitDecision{ false, false, -1 };
    }

    bool add    = true;
    int  delIdx = -1;

    for (std::size_t j = 0; j < existing.size(); ++j)
    {
        const auto& ex = existing[j];
        if (ex.id != candidate.id)
        {
            continue;
        }
        if (isPC && ex.meritID > 0)
        {
            if (meritCountExisting(ex.meritID) == 0)
            {
                delIdx = static_cast<int>(j);
                break;
            }
            else if (ex.meritID == candidate.meritID)
            {
                add = false;
                break;
            }
        }
        if (ex.rank < candidate.rank)
        {
            delIdx = static_cast<int>(j);
            break;
        }
        else if (ex.rank > candidate.rank)
        {
            add = false;
            break;
        }
        else if (ex.mod == candidate.mod)
        {
            add = false;
            break;
        }
    }

    if (isPC && candidate.meritID > 0 && meritCountCandidate(candidate.meritID) == 0)
    {
        add = false;
    }

    return TraitDecision{ true, add, delIdx };
}

constexpr auto ShouldGenerateCureEnmity(const bool hasSource, const bool hasTarget) -> bool
{
    return hasSource && hasTarget;
}

constexpr auto ShouldUpdateCureEnmity(const bool isMob, const std::uint8_t hiPCLvl, const bool hasTargetID) -> bool
{
    return isMob && hiPCLvl > 0 && hasTargetID;
}

constexpr auto ResolveInRangeEnmitySource(const bool sourceIsPC, const bool hasMaster, const bool masterIsPC, bool& useSource, bool& useMaster) -> void
{
    useSource = false;
    useMaster = false;
    if (sourceIsPC)
    {
        useSource = true;
        return;
    }
    if (hasMaster && masterIsPC)
    {
        useMaster = true;
    }
}

constexpr auto ShouldUpdateInRangeEnmity(const std::uint8_t hiPCLvl, const bool hasSourceID) -> bool
{
    return hiPCLvl > 0 && hasSourceID;
}

constexpr auto ShouldGenerateInRangeEnmity(const bool hasSource) -> bool
{
    return hasSource;
}

} // namespace traitsenmityhelpers
