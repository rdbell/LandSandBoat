#pragma once

#include <cstdint>
#include <functional>
#include <utility>

// Pure CCharEntity packet-queue admission / OnPush / OnPop policy halves.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1468: CharacterPacketQueue host surface
//   - 2840: Filtered (0x70 synthesis filter)
//   - 2842: ShouldSetPendingPositionOnPush / OnPush
//   - 2845: ShouldEraseEntityUpdateOnPop + ShouldClearPendingPositionOnPop / OnPop
//   - 2943: ShouldClearPendingPositionOnPop residual dual-wire
//           (packetType == 0x5B && packetEntityID == ownerID)
//   - 3105: ShouldEraseEntityUpdateOnPop residual dual-wire
//           (packetType == 0x0D || packetType == 0x0E)
//
// Production host: CCharEntity::popPacket (char_entity.cpp) injects type /
// entity-id / owner scalars into OnPop, which dual-wires erase through
// ShouldEraseEntityUpdateOnPop and clear-pending through
// ShouldClearPendingPositionOnPop.
// Go dual-wire erase: charentity.ShouldEraseEntityUpdateOnPop
// (internal/charentity/erase_entity_update_pop.go).
// Go dual-wire clear-pending: charentity.ShouldClearPendingPositionOnPop
// (internal/charentity/clear_pending_pop.go).

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
//
// Formula (slice 3105 dual-wire; residual pure port from slice 2845):
//   packetType == 0x0D || packetType == 0x0E
//
// packetType — host-evaluated packet->getType()
// true  → eraseEntityUpdate(packetEntityID) / EntityUpdatePackets.erase
// false → leave entity-update map unchanged on this branch
//
// Host dual-wires (OnPop template if-branch before clear-pending):
//   if (ShouldEraseEntityUpdateOnPop(packetType))
//       eraseEntityUpdate(packetEntityID);
//
// Dual-wire of Go charentity.ShouldEraseEntityUpdateOnPop
// (internal/charentity/erase_entity_update_pop.go).
// Call site: CCharEntity::popPacket → OnPop after entity-id extract
// (0x0D/0x0E → ref 0x04). Mutual exclusion with ShouldClearPendingPositionOnPop:
// erase types never also clear pending (if / else if).
inline bool ShouldEraseEntityUpdateOnPop(const std::uint16_t packetType)
{
    return packetType == 0x0D || packetType == 0x0E;
}

// ShouldClearPendingPositionOnPop is the pure gate for OnPop clear-pending:
// true when the packet is a position update (0x5B) for the owning character.
//
// Formula (slice 2943 dual-wire; residual pure port from slice 2845):
//   packetType == 0x5B && packetEntityID == ownerID
//
// packetType     — host-evaluated packet->getType()
// packetEntityID — host-injected entity id (0x5B → ref 0x10; else 0 for this path)
// ownerID        — host-injected owning character id (this->id)
// true  → setPending(false) / clear pendingPositionUpdate
// false → leave pending flag unchanged on this branch
//
// Host dual-wires (OnPop template else-if after erase gate):
//   else if (ShouldClearPendingPositionOnPop(packetType, packetEntityID, ownerID))
//       setPending(false);
//
// Dual-wire of Go charentity.ShouldClearPendingPositionOnPop
// (internal/charentity/clear_pending_pop.go).
// Call site: CCharEntity::popPacket → OnPop after entity-id extract.
// Mutual exclusion with ShouldEraseEntityUpdateOnPop: erase types never also
// clear pending (if / else if).
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
