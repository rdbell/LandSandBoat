#include "test_map_ipc_diagnostics_1305.h"

#include "common/ipc.h"
#include "map/ipc_diagnostics.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map IPC diagnostics 1305 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapIPCDiagnostics1305SelfTests() -> bool
{
    const IPP source{ str2ip("1.2.3.4"), std::numeric_limits<uint16>::max() };

    const bool emptyText = expect(mapipc::FormatEmptyStructWarning(source) ==
                                      "Received EmptyStruct message from 1.2.3.4:65535 - this is probably a bug",
                                  "EmptyStruct warning text");
    const bool unknownText = expect(mapipc::FormatUnknownMessageWarning(source, std::numeric_limits<uint8>::max(), 4) ==
                                        "Received unknown message from 1.2.3.4:65535 with code 255 and size 4",
                                    "unknown warning text");
    const auto wrappedSize = static_cast<std::size_t>(-1);
    const bool wrappedText = expect(mapipc::FormatUnknownMessageWarning(source, 0x01, wrappedSize) ==
                                        "Received unknown message from 1.2.3.4:65535 with code 1 and size " +
                                            std::to_string(wrappedSize),
                                    "size_t message length");

    const IPP mid{ str2ip("5.6.7.8"), 54000 };
    const bool midText = expect(mapipc::FormatEmptyStructWarning(mid) ==
                                    "Received EmptyStruct message from 5.6.7.8:54000 - this is probably a bug",
                                "mid-range IPP formatting") &&
                         expect(mapipc::FormatUnknownMessageWarning(mid, 0x00, 1) ==
                                    "Received unknown message from 5.6.7.8:54000 with code 0 and size 1",
                                "zero code and size one");

    const std::vector<uint8> headerOnly{ static_cast<uint8>(ipc::MessageType::EmptyStruct) };
    const auto               decoded = ipc::fromBytesWithHeader<ipc::EmptyStruct>(headerOnly);
    const bool               headerRejected = expect(!decoded.has_value(), "header-only EmptyStruct rejected");

    return emptyText && unknownText && wrappedText && midText && headerRejected;
}
