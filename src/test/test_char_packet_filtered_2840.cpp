#include "test_char_packet_filtered_2840.h"

#include "map/char_packet_queue_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "char packet filtered 2840 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runCharPacketFiltered2840SelfTests() -> bool
{
    using charpacketqueuehelpers::Filtered;

    bool ok = true;

    // Type 0x70 (synthesis/fishing results) is filtered only when the flag is on.
    ok = expect(Filtered(0x70, true), "0x70 filter on") && ok;
    ok = expect(!Filtered(0x70, false), "0x70 filter off") && ok;

    // Other packet types are never filtered by this gate.
    ok = expect(!Filtered(0x6F, true), "0x6F filter on") && ok;
    ok = expect(!Filtered(0x71, true), "0x71 filter on") && ok;
    ok = expect(!Filtered(0x00, true), "0x00 filter on") && ok;
    ok = expect(!Filtered(0x5B, true), "0x5B filter on") && ok;
    ok = expect(!Filtered(0x0D, true), "0x0D filter on") && ok;
    ok = expect(!Filtered(0x0E, false), "0x0E filter off") && ok;

    // Table pin: pure predicate is exactly (type == 0x70 && filterOthersSynthesis).
    const struct
    {
        std::uint16_t packetType;
        bool          filterOthersSynthesis;
        bool          want;
        const char*   label;
    } cases[] = {
        { 0x70, true, true, "compose 0x70 on" },
        { 0x70, false, false, "compose 0x70 off" },
        { 0x6F, true, false, "compose neighbor on" },
        { 0x71, false, false, "compose neighbor off" },
    };
    for (const auto& c : cases)
    {
        const bool got = Filtered(c.packetType, c.filterOthersSynthesis);
        ok             = expect(got == c.want, c.label) && ok;
        ok             = expect(got == (c.packetType == 0x70 && c.filterOthersSynthesis), "compose identity") && ok;
    }

    return ok;
}
