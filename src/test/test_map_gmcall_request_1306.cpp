#include "test_map_gmcall_request_1306.h"

#include "common/ipc.h"
#include "map/gmcall_request.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map GMCallRequest 1306 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapGMCallRequest1306SelfTests() -> bool
{
    // The pure seam is intentionally a no-op. Characterization pins that the
    // production-callable entry exists, accepts empty and fully-populated
    // payloads, and does not throw for boundary field values.
    const IPP source{ str2ip("1.2.3.4"), std::numeric_limits<uint16>::max() };

    const ipc::GMCallRequest empty{};
    mapipc::HandleGMCallRequest(source, empty);

    const ipc::GMCallRequest full{
        .callId     = std::numeric_limits<uint32>::max(),
        .charId     = 1,
        .charName   = "TestChar",
        .accId      = 2,
        .zoneId     = 3,
        .posX       = 1.25f,
        .posY       = -2.5f,
        .posZ       = 3.75f,
        .message    = "help {with} %s",
        .parameters = { { "category", "stuck" }, { "area", "Mog House" } },
    };
    mapipc::HandleGMCallRequest(source, full);

    const IPP mid{ str2ip("5.6.7.8"), 54000 };
    mapipc::HandleGMCallRequest(mid, full);

    // Encode uses the same production helper as live IPC; wire shape is already
    // golden-tested in the GMCall payload suite. Header-only frames are rejected
    // before any map handler runs.
    const auto encoded = ipc::toBytesWithHeader(full);
    const bool encodedNonEmpty = expect(!encoded.empty() && encoded[0] == static_cast<uint8>(ipc::MessageType::GMCallRequest),
                                        "GMCallRequest encodes with type header");

    const std::vector<uint8> headerOnly{ static_cast<uint8>(ipc::MessageType::GMCallRequest) };
    const auto               headerDecoded = ipc::fromBytesWithHeader<ipc::GMCallRequest>(headerOnly);
    const bool headerRejected = expect(!headerDecoded.has_value(), "header-only GMCallRequest rejected");

    return encodedNonEmpty && headerRejected;
}
