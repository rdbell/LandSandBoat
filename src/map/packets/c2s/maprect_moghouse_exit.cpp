#include "maprect_moghouse_exit.h"

#include <array>
#include <ranges>

namespace
{

constexpr uint16_t ZoneSouthernSandoria = 230;
constexpr uint16_t ZoneBastokMines      = 234;
constexpr uint16_t ZoneWindurstWaters   = 238;
constexpr uint16_t ZoneRuludeGardens    = 243;
constexpr uint16_t ZoneAlZahbi          = 48;
constexpr uint16_t ZoneWhitegate        = 50;
constexpr uint16_t ZoneWesternAdoulin   = 256;
constexpr uint16_t ZoneEasternAdoulin   = 257;
constexpr uint16_t ZoneMogGarden        = 280;

auto bytesFor(const uint32_t rectID) -> std::array<char, 4>
{
    return {
        static_cast<char>(rectID),
        static_cast<char>(rectID >> 8),
        static_cast<char>(rectID >> 16),
        static_cast<char>(rectID >> 24),
    };
}

auto isMogHouseExitRegion(const REGION_TYPE region) -> bool
{
    constexpr auto accepted = std::array{
        REGION_TYPE::SANDORIA,
        REGION_TYPE::BASTOK,
        REGION_TYPE::WINDURST,
        REGION_TYPE::JEUNO,
        REGION_TYPE::WEST_AHT_URHGAN,
        REGION_TYPE::ADOULIN_ISLANDS,
    };
    return std::ranges::any_of(accepted, [region](const auto acceptedRegion)
                               {
                                   return region == acceptedRegion;
                               });
}

} // namespace

auto maprect::ClassifyRectID(const uint32_t rectID) -> RectIDClassification
{
    const auto bytes = bytesFor(rectID);
    return {
        .isMogHouseExit     = bytes == std::array{ 'z', 'm', 'r', 'q' },
        .isMogHouseEntrance = bytes[0] == 'z' && bytes[1] == 'm' && (bytes[2] == 'r' || bytes[2] == 's'),
    };
}

auto maprect::MogHouseExitFor(const MogHouseExitInput& input) -> MogHouseExitDecision
{
    auto decision = MogHouseExitDecision{ .destinationZone = input.startingZone };
    if (!input.regionForZone)
    {
        return decision;
    }

    switch (input.exitMode)
    {
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::AreaEnteredFrom:
            break;
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option1:
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option2:
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option3:
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option4:
            switch (input.exitBit)
            {
                case GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::SandOria:
                    decision.destinationZone = static_cast<uint8_t>(input.exitMode) + ZoneSouthernSandoria - 1;
                    break;
                case GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Bastok:
                    decision.destinationZone = static_cast<uint8_t>(input.exitMode) + ZoneBastokMines - 1;
                    break;
                case GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Windurst:
                    decision.destinationZone = static_cast<uint8_t>(input.exitMode) + ZoneWindurstWaters - 1;
                    break;
                case GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Jeuno:
                    decision.destinationZone = static_cast<uint8_t>(input.exitMode) + ZoneRuludeGardens - 1;
                    break;
                case GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Whitegate:
                    decision.destinationZone = static_cast<uint8_t>(input.exitMode) + (input.exitMode == GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option1 ? ZoneAlZahbi - 1 : ZoneWhitegate - 2);
                    break;
                case GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Adoulin:
                    decision.destinationZone = input.exitMode == GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Option2 ? ZoneEasternAdoulin : ZoneWesternAdoulin;
                    break;
                default:
                    decision.action = MogHouseExitAction::Abort;
                    return decision;
            }
            break;
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Mog1F:
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Mog2F:
            break;
        case GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::MogGarden:
            decision.destinationZone = ZoneMogGarden;
            break;
        default:
            return decision;
    }

    const auto requestedFloor = decision.destinationZone == input.startingZone &&
                                (input.exitMode == GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Mog1F || input.exitMode == GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::Mog2F);
    if (requestedFloor)
    {
        if ((input.mogHouseFlags & 0x20) == 0)
        {
            return decision;
        }
        decision.action                   = MogHouseExitAction::ChangeFloor;
        decision.resetPosition            = true;
        decision.toggleSecondFloorTracker = true;
        return decision;
    }

    const auto regular           = input.exitMode == GP_CLI_COMMAND_MAPRECT_MYROOMEXITMODE::AreaEnteredFrom && input.inMogHouse;
    const auto destinationRegion = input.regionForZone(decision.destinationZone);
    const auto questBit          = input.exitBit == GP_CLI_COMMAND_MAPRECT_MYROOMEXITBIT::Default ? 0 : uint16_t(1) << (static_cast<uint8_t>(input.exitBit) - 1);
    const auto questComplete     = input.startingRegion == REGION_TYPE::ADOULIN_ISLANDS || (input.mogHouseFlags & questBit) != 0;
    const auto questExit         = questComplete && input.startingRegion == destinationRegion && input.inMogHouse && isMogHouseExitRegion(destinationRegion);
    const auto gardenExit        = decision.destinationZone == ZoneMogGarden && input.inMogHouse;
    if (!regular && !questExit && !gardenExit)
    {
        return decision;
    }

    decision.action                  = MogHouseExitAction::LeaveMogHouse;
    decision.clearMogHouseID         = true;
    decision.resetPosition           = true;
    decision.clearSecondFloorTracker = true;
    return decision;
}
