#include "test_world_account_login_1252.h"

#include "world/account_login.h"

#include "common/ipp.h"

#include <iostream>
#include <utility>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world AccountLogin 1252 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testExactOrderedFanout() -> bool
{
    const auto first     = IPP(0x0100007F, 54230);
    const auto second    = IPP(0x0200007F, 54231);
    const auto message   = ipc::AccountLogin{ .accountId = 0x00345678 };
    const auto endpoints = std::vector<IPP>{ first, second, first };
    auto       sent      = std::vector<std::pair<uint64, uint32>>{};

    worldipc::HandleAccountLogin(
        message,
        endpoints,
        [&](const IPP& endpoint, const ipc::AccountLogin& delivered)
        {
            sent.emplace_back(endpoint.getRawIPP(), delivered.accountId);
        });

    return expect(sent.size() == 3, "one send per endpoint entry") &&
           expect(sent[0] == std::pair{ first.getRawIPP(), message.accountId }, "first endpoint/payload") &&
           expect(sent[1] == std::pair{ second.getRawIPP(), message.accountId }, "second endpoint/payload") &&
           expect(sent[2] == std::pair{ first.getRawIPP(), message.accountId }, "duplicate endpoint preserved");
}

auto testEmptyEndpoints() -> bool
{
    int sends = 0;
    worldipc::HandleAccountLogin(
        ipc::AccountLogin{ .accountId = 7 },
        std::vector<IPP>{},
        [&](const IPP&, const ipc::AccountLogin&)
        {
            ++sends;
        });
    return expect(sends == 0, "empty endpoint list is a no-op");
}

} // namespace

auto runWorldAccountLogin1252SelfTests() -> bool
{
    return testExactOrderedFanout() && testEmptyEndpoints();
}
