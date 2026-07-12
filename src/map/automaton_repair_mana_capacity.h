#pragma once

#include <cstdint>
#include <optional>

// Pure xi.automaton.repairKit / manaTank max HP/MP boost tables used by
// LoadAutomatonStats.
// Parity: internal/automaton repair_mana.go (slice 1587).
// Production wire: petutils.cpp LoadAutomatonStats.

namespace automatonrepairmanahelpers
{

// Attachment IDs (AutomatonAttachment enum).
constexpr std::uint8_t AutoRepairKit    = 193;
constexpr std::uint8_t AutoRepairKitII  = 196;
constexpr std::uint8_t AutoRepairKitIII = 202;
constexpr std::uint8_t AutoRepairKitIV  = 205;
constexpr std::uint8_t ManaTank         = 225;
constexpr std::uint8_t ManaTankII       = 228;
constexpr std::uint8_t ManaTankIII      = 233;
constexpr std::uint8_t ManaTankIV       = 235;

// Frame values (AutomatonFrame enum).
constexpr std::uint8_t FrameHarlequin  = 0x20;
constexpr std::uint8_t FrameValoredge  = 0x21;
constexpr std::uint8_t FrameSharpshot  = 0x22;
constexpr std::uint8_t FrameStormwaker = 0x23;

constexpr auto RepairKitHPBoost(const std::uint8_t attachmentId) -> std::uint8_t
{
    switch (attachmentId)
    {
        case AutoRepairKit:
            return 1;
        case AutoRepairKitII:
            return 2;
        case AutoRepairKitIII:
            return 3;
        case AutoRepairKitIV:
            return 4;
        default:
            return 0;
    }
}

constexpr auto ManaTankMPBoost(const std::uint8_t attachmentId) -> std::uint8_t
{
    switch (attachmentId)
    {
        case ManaTank:
            return 1;
        case ManaTankII:
            return 2;
        case ManaTankIII:
            return 3;
        case ManaTankIV:
            return 4;
        default:
            return 0;
    }
}

// xi.automaton.repairKit.frameDivisors
inline auto RepairKitFrameDivisor(const std::uint8_t frame) -> std::optional<std::uint16_t>
{
    switch (frame)
    {
        case FrameHarlequin:
            return 20;
        case FrameValoredge:
            return 24;
        case FrameSharpshot:
            return 18;
        case FrameStormwaker:
            return 16;
        default:
            return std::nullopt;
    }
}

// xi.automaton.manaTank.frameDivisors (Harlequin + Stormwaker only).
inline auto ManaTankFrameDivisor(const std::uint8_t frame) -> std::optional<std::uint16_t>
{
    switch (frame)
    {
        case FrameHarlequin:
            return 20;
        case FrameStormwaker:
            return 24;
        default:
            return std::nullopt;
    }
}

// pool += pool * tier / divisor (integer division).
constexpr auto ApplyAttachmentPoolBoost(const std::int32_t pool, const std::uint8_t tier, const std::uint16_t divisor) -> std::int32_t
{
    if (divisor == 0)
    {
        return pool;
    }
    return pool + pool * static_cast<std::int32_t>(tier) / static_cast<std::int32_t>(divisor);
}

template <typename HasAttachmentFn>
inline auto SumRepairKitTier(HasAttachmentFn&& hasAttachment) -> std::uint8_t
{
    std::uint8_t tier = 0;
    for (const std::uint8_t id : { AutoRepairKit, AutoRepairKitII, AutoRepairKitIII, AutoRepairKitIV })
    {
        if (hasAttachment(id))
        {
            tier += RepairKitHPBoost(id);
        }
    }
    return tier;
}

template <typename HasAttachmentFn>
inline auto SumManaTankTier(HasAttachmentFn&& hasAttachment) -> std::uint8_t
{
    std::uint8_t tier = 0;
    for (const std::uint8_t id : { ManaTank, ManaTankII, ManaTankIII, ManaTankIV })
    {
        if (hasAttachment(id))
        {
            tier += ManaTankMPBoost(id);
        }
    }
    return tier;
}

} // namespace automatonrepairmanahelpers
