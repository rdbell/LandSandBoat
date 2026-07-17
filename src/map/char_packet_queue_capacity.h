#pragma once

#include <cstdint>
#include <functional>
#include <utility>

namespace charpacketqueuehelpers
{

inline bool Filtered(const std::uint16_t packetType, const bool filterOthersSynthesis)
{
    return packetType == 0x70 && filterOthersSynthesis;
}

// ShouldSetPendingPositionOnPush is the pure admission gate for OnPush pending
// position: true when the packet is a position update (0x5B) for the owning
// character. Host dual-wires:
//   if (ShouldSetPendingPositionOnPush(packetType, packetEntityID, ownerID))
//       setPending(true);
inline bool ShouldSetPendingPositionOnPush(const std::uint16_t packetType,
                                           const std::uint32_t packetEntityID,
                                           const std::uint32_t ownerID)
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

template <typename SetPending>
inline void OnPush(const std::uint16_t packetType, const std::uint32_t packetEntityID, const std::uint32_t ownerID, SetPending&& setPending)
{
    if (ShouldSetPendingPositionOnPush(packetType, packetEntityID, ownerID))
    {
        std::invoke(std::forward<SetPending>(setPending), true);
    }
}

// ShouldEraseEntityUpdateOnPop is the pure gate for OnPop entity-update
// cleanup: true when the packet is a char/entity update (0x0D or 0x0E).
// Host dual-wires:
//   if (ShouldEraseEntityUpdateOnPop(packetType))
//       eraseEntityUpdate(packetEntityID);
inline bool ShouldEraseEntityUpdateOnPop(const std::uint16_t packetType)
{
    return packetType == 0x0D || packetType == 0x0E;
}

// ShouldClearPendingPositionOnPop is the pure gate for OnPop clear-pending:
// true when the packet is a position update (0x5B) for the owning character.
// Host dual-wires:
//   else if (ShouldClearPendingPositionOnPop(packetType, packetEntityID, ownerID))
//       setPending(false);
inline bool ShouldClearPendingPositionOnPop(const std::uint16_t packetType,
                                            const std::uint32_t packetEntityID,
                                            const std::uint32_t ownerID)
{
    return packetType == 0x5B && packetEntityID == ownerID;
}

template <typename EraseEntityUpdate, typename SetPending>
inline void OnPop(const std::uint16_t packetType,
                  const std::uint32_t packetEntityID,
                  const std::uint32_t ownerID,
                  EraseEntityUpdate&& eraseEntityUpdate,
                  SetPending&&        setPending)
{
    if (ShouldEraseEntityUpdateOnPop(packetType))
    {
        std::invoke(std::forward<EraseEntityUpdate>(eraseEntityUpdate), packetEntityID);
    }
    else if (ShouldClearPendingPositionOnPop(packetType, packetEntityID, ownerID))
    {
        std::invoke(std::forward<SetPending>(setPending), false);
    }
}

} // namespace charpacketqueuehelpers
