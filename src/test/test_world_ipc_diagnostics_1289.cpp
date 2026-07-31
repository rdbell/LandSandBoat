#include "test_world_ipc_diagnostics_1289.h"
#include "common/ipc.h"
#include "world/ipc_diagnostics.h"
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
namespace
{
auto expect(bool condition, const char* label) -> bool
{
    if (!condition) std::cerr << "world IPC diagnostics 1289 self-test failed: " << label << '\n';
    return condition;
}
} // namespace
auto runWorldIPCDiagnostics1289SelfTests() -> bool
{
    const IPP source{ str2ip("1.2.3.4"), std::numeric_limits<uint16>::max() };
    bool      ok = true;

    ok = expect(worldipc::FormatEmptyStructWarning(source) ==
                    "Received EmptyStruct message from 1.2.3.4:65535 - this is probably a bug",
                "EmptyStruct warning text") &&
         ok;
    ok = expect(worldipc::FormatUnknownMessageWarning(source, std::numeric_limits<uint8>::max(), 4) ==
                    "Received unknown message from 1.2.3.4:65535 with code 255 and size 4",
                "unknown warning text") &&
         ok;
    const auto wrappedSize = static_cast<std::size_t>(-1);
    ok                     = expect(worldipc::FormatUnknownMessageWarning(source, 0x01, wrappedSize) ==
                                        "Received unknown message from 1.2.3.4:65535 with code 1 and size " +
                                            std::to_string(wrappedSize),
                                    "size_t message length") &&
         ok;

    const std::vector<uint8> headerOnly{ static_cast<uint8>(ipc::MessageType::EmptyStruct) };
    const auto               decoded = ipc::fromBytesWithHeader<ipc::EmptyStruct>(headerOnly);
    ok = expect(!decoded.has_value(), "header-only EmptyStruct rejected") && ok;

    return ok;
}
