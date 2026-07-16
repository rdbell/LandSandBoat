#pragma once

#include <cstdint>

#include "0x05e_maprect.h"
#include "map/zone.h"

namespace maprect
{

struct RectIDClassification
{
    bool isMogHouseExit{};
    bool isMogHouseEntrance{};
};

// ClassifyRectID interprets the wire fourcc as exactly four bytes; it never
// relies on a trailing NUL.
auto ClassifyRectID(uint32_t rectID) -> RectIDClassification;

enum class MogHouseExitAction : uint8_t
{
    LeaveMogHouse,
    ChangeFloor,
    Denied,
    Abort,
};

struct MogHouseExitInput
{
    uint16_t                              startingZone{};
    REGION_TYPE                           startingRegion{ REGION_TYPE::UNKNOWN };
    bool                                  inMogHouse{};
    uint16_t                              mogHouseFlags{};
    GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT  exitBit{};
    GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE exitMode{};
    REGION_TYPE (*regionForZone)(uint16_t){};
};

struct MogHouseExitDecision
{
    MogHouseExitAction action{ MogHouseExitAction::Denied };
    uint16_t           destinationZone{};
    bool               clearMogHouseID{};
    bool               resetPosition{};
    bool               clearSecondFloorTracker{};
    bool               toggleSecondFloorTracker{};
};

// MogHouseExitFor selects and authorizes an exit after a zmrq fourcc. Zone
// lookup, entity mutation, and generic zoneline handling stay host-owned.
auto MogHouseExitFor(const MogHouseExitInput& input) -> MogHouseExitDecision;

} // namespace maprect
