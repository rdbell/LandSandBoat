#include "test_gmcall_assembly_1238.h"

#include "map/gmcall_container.h"

#include <cstring>
#include <iostream>
#include <string_view>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "GM-call assembly 1238 self-test failed: " << label << '\n';
    }
    return condition;
}

template <typename T>
void appendStruct(GP_CLI_COMMAND_FAQ_GMCALL& packet, std::size_t& offset, const T& value)
{
    std::memcpy(packet.Data + offset, &value, sizeof(value));
    offset += sizeof(value);
    ++packet.blkNum;
}

void appendString(GP_CLI_COMMAND_FAQ_GMCALL& packet, std::size_t& offset, std::string_view value)
{
    FFGpGMReportBlockHdr header{
        .bkType   = static_cast<uint8_t>(GMReportBlockType::StringParam),
        .bkLength = static_cast<uint8_t>(sizeof(FFGpGMReportBlockHdr) + value.size()),
        .bkOpt    = 0,
    };
    std::memcpy(packet.Data + offset, &header, sizeof(header));
    std::memcpy(packet.Data + offset + sizeof(header), value.data(), value.size());
    offset += header.bkLength;
    ++packet.blkNum;
}

auto testSupportedBlocksAndSequenceOrdering() -> bool
{
    GP_CLI_COMMAND_FAQ_GMCALL first{};
    first.seq               = 0;
    std::size_t firstOffset = 0;
    appendStruct(first, firstOffset, sub_block_01_t{
                                         .header    = { static_cast<uint8_t>(GMReportBlockType::Version), sizeof(sub_block_01_t), 0 },
                                         .unknown00 = 42,
                                         .unknown01 = { 1, 2, 3, 4 },
                                     });
    appendStruct(first, firstOffset, FFGpGMReportECodeStruct{
                                         .header   = { static_cast<uint8_t>(GMReportBlockType::Error), sizeof(FFGpGMReportECodeStruct), 0 },
                                         .code     = -7,
                                         .count    = 8,
                                         .timeCode = 9,
                                     });
    appendString(first, firstOffset, "GMCALL.INPUT:early");

    GP_CLI_COMMAND_FAQ_GMCALL middle{};
    middle.seq                           = 1;
    std::size_t             middleOffset = 0;
    FFGpGMReportLobbyStruct lobby{
        .header = { static_cast<uint8_t>(GMReportBlockType::LobbyHistory),
                    static_cast<uint8_t>(sizeof(FFGpGMReportBlockHdr) + 2 * sizeof(FFGpGMReportLobbyEntry)),
                    0 },
    };
    lobby.characters[0].ident    = 0;
    lobby.characters[1].cmd      = 10;
    lobby.characters[1].opt      = 11;
    lobby.characters[1].timeCode = 12;
    lobby.characters[1].ident    = 13;
    std::memcpy(lobby.characters[1].name, "Alice", 5);
    std::memcpy(middle.Data + middleOffset, &lobby, lobby.header.bkLength);
    middleOffset += lobby.header.bkLength;
    ++middle.blkNum;
    appendString(middle, middleOffset, "GENERIC.VERSION:v:1");
    appendString(middle, middleOffset, "UNKNOWN.KEY:ignored");

    GP_CLI_COMMAND_FAQ_GMCALL last{};
    last.seq               = 2;
    std::size_t lastOffset = 0;
    appendString(last, lastOffset, "GMCALL.INPUT:late");

    const auto call = gmcall::detail::AssembleCall({ last, middle, first });

    bool ok = true;
    ok      = expect(call.message == "late", "sequence ordering selects last message") && ok;
    ok      = expect(call.parameters.at("GMCALL.INPUT") == "late", "message parameter overwritten in sequence order") && ok;
    ok      = expect(call.parameters.at("GENERIC.VERSION") == "v:1", "string parameter splits at first colon") && ok;
    ok      = expect(!call.parameters.contains("UNKNOWN.KEY"), "unknown parameter ignored") && ok;
    ok      = expect(call.parameters.at("VERSION.UNKNOWN00") == "42", "version scalar") && ok;
    ok      = expect(call.parameters.at("VERSION.UNKNOWN01") == "1,2,3,4", "version tuple") && ok;
    ok      = expect(call.parameters.at("ERROR.CODE") == "-7" && call.parameters.at("ERROR.COUNT") == "8" &&
                         call.parameters.at("ERROR.TIMECODE") == "9",
                     "error fields") &&
              ok;
    ok      = expect(!call.parameters.contains("LOBBY.0.IDENT"), "zero-ident lobby entry skipped") && ok;
    ok      = expect(call.parameters.at("LOBBY.1.CMD") == "10" && call.parameters.at("LOBBY.1.OPT") == "11" &&
                         call.parameters.at("LOBBY.1.TIMECODE") == "12" && call.parameters.at("LOBBY.1.IDENT") == "13" &&
                         call.parameters.at("LOBBY.1.NAME") == "Alice",
                     "lobby entry fields") &&
              ok;
    return ok;
}

auto testMalformedBlockStopsOnlyItsPacket() -> bool
{
    GP_CLI_COMMAND_FAQ_GMCALL malformed{};
    malformed.seq     = 0;
    malformed.blkNum  = 2;
    malformed.Data[0] = static_cast<uint8_t>(GMReportBlockType::StringParam);
    malformed.Data[1] = 3; // Smaller than FFGpGMReportBlockHdr.

    GP_CLI_COMMAND_FAQ_GMCALL valid{};
    valid.seq          = 1;
    std::size_t offset = 0;
    appendString(valid, offset, "GMCALL.INPUT:survives");

    const auto call = gmcall::detail::AssembleCall({ valid, malformed });
    return expect(call.message == "survives" && call.parameters.size() == 1, "malformed packet does not suppress later packet");
}

auto testShortKnownAndUnknownBlocksAdvance() -> bool
{
    GP_CLI_COMMAND_FAQ_GMCALL packet{};
    packet.seq         = 0;
    std::size_t offset = 0;

    const FFGpGMReportBlockHdr shortVersion{
        .bkType   = static_cast<uint8_t>(GMReportBlockType::Version),
        .bkLength = sizeof(FFGpGMReportBlockHdr),
        .bkOpt    = 0,
    };
    appendStruct(packet, offset, shortVersion);
    const FFGpGMReportBlockHdr unknown{
        .bkType   = 0xFE,
        .bkLength = sizeof(FFGpGMReportBlockHdr),
        .bkOpt    = 0,
    };
    appendStruct(packet, offset, unknown);
    appendString(packet, offset, "GMCALL.INPUT:after");

    const auto call = gmcall::detail::AssembleCall({ packet });
    return expect(call.message == "after" && call.parameters.size() == 1, "short known and unknown blocks advance") &&
           expect(!call.parameters.contains("VERSION.UNKNOWN00"), "short version ignored");
}

} // namespace

auto runGMCallAssembly1238SelfTests() -> bool
{
    bool ok = true;
    ok      = testSupportedBlocksAndSequenceOrdering() && ok;
    ok      = testMalformedBlockStopsOnlyItsPacket() && ok;
    ok      = testShortKnownAndUnknownBlocksAdvance() && ok;
    return ok;
}
