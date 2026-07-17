#pragma once

// Pure VIEW 0x24 world-list response selection. Packet framing stays in
// login_packets / GenerateWorldList; this only decides shape/write targets.

namespace login
{

struct view_world_list_response_plan
{
    bool shapeWorldListPacket{};
    bool writeWorldListPacket{};
};

// PlanViewWorldListResponse mirrors view_session's 0x24 branch. The world-list
// packet is always shaped; it is written only when a view peer is present.
inline auto PlanViewWorldListResponse(const bool hasViewSession) -> view_world_list_response_plan
{
    return {
        .shapeWorldListPacket = true,
        .writeWorldListPacket = hasViewSession,
    };
}

} // namespace login
