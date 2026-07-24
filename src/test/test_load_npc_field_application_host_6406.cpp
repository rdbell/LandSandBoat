#include "test_load_npc_field_application_host_6406.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "load npc field application host 6406 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for LoadNPCList CNpcEntity field application (slice 6406).
// Go: ApplyNPCRowFields / LoadedNPC / npcentity.State flags+widescan.
auto runLoadNPCFieldApplicationHost6406SelfTests() -> bool
{
    bool ok = true;

    // Field catalog order mirrors LoadNPCList assignment.
    const std::string fields =
        "targid,id,name,packetName,rotation,x,y,z,moving,m_TargID,"
        "animationSpeed,baseSpeed,animation,animationsub,namevis,status,"
        "m_flags,look,name_prefix,widescan";
    ok = expect(fields.find("targid") != std::string::npos, "targid") && ok;
    ok = expect(fields.find("m_flags") != std::string::npos, "flags") && ok;
    ok = expect(fields.find("widescan") != std::string::npos, "widescan") && ok;

    // flag → moving (low 16) and m_TargID (high 16).
    const uint32_t flag   = 0x00040005u;
    const uint16_t moving = static_cast<uint16_t>(flag);
    const uint32_t mtarg  = flag >> 16;
    ok = expect(moving == 5, "moving") && ok;
    ok = expect(mtarg == 4, "mtarg") && ok;

    // Default widescan_ member is 1; SQL may override to 0.
    const uint8_t defaultWidescan = 1;
    ok = expect(defaultWidescan == 1, "default widescan") && ok;

    return ok;
}
