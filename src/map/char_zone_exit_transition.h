#pragma once

#include <cstdint>

namespace zoneexithelpers
{
enum class ZoneExitKind : std::uint8_t { Disconnect, Rezone };
enum class Destination : std::uint8_t { Invalid, Current };
struct ZoneExitPlan
{
    bool savePosition{};
    bool clearPackets{};
    Destination destination{};
    bool setShutdownStatus{};
    bool setDisappearStatus{};
    bool clearBoundary{};
    bool requestZoneChange{};
    bool savePetZoningInfo{};
    bool sendLogoutPacket{};
    constexpr auto operator==(const ZoneExitPlan&) const -> bool = default;
};
constexpr auto MakeZoneExitPlan(const ZoneExitKind kind, const bool petPersists) -> ZoneExitPlan
{
    if (kind == ZoneExitKind::Disconnect) return { .savePosition = true, .clearPackets = true, .destination = Destination::Invalid, .setShutdownStatus = true, .requestZoneChange = true, .savePetZoningInfo = petPersists, .sendLogoutPacket = true };
    return { .clearPackets = true, .destination = Destination::Current, .setDisappearStatus = true, .clearBoundary = true, .requestZoneChange = true, .savePetZoningInfo = petPersists };
}
} // namespace zoneexithelpers
