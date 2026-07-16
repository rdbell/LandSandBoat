#include "test_s2c_message_runtime.h"

#include <cstring>
#include <iostream>

#include "map/packets/s2c/message_runtime.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "s2c MESSAGE runtime self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runS2CMessageRuntimeSelfTests() -> bool
{
    using namespace messagehelpers;
    const auto character = CharacterFacts{ .present = true, .uniqueNo = 0x11223344, .actIndex = 0x5566, .name = "Aldo" };

    bool ok = true;
    const auto ordinary = CharacterParams2PlanFor(character, 33, 44, MsgStd::CannotInvite);
    ok = expect(ordinary.size == 0x24 && ordinary.packet.UniqueNo == 0x11223344 && ordinary.packet.ActIndex == 0x5566 && ordinary.packet.Attr == 0 && ordinary.packet.Data[0] == '\0', "ordinary character message preserves fixed size and ignores params") && ok;
    const auto examine = CharacterParams2PlanFor(character, 33, 44, MsgStd::Examine);
    ok = expect(examine.size == 0x60 && examine.packet.Attr == 0x10 && std::strcmp(examine.packet.Data, "string2 Aldo") == 0, "examine takes size and attr precedence") && ok;
    const auto checkIn = CharacterParams2PlanFor(character, 33, 44, MsgStd::MonstrosityCheckIn);
    const auto checkOut = CharacterParams2PlanFor(character, 33, 44, MsgStd::MonstrosityCheckOut);
    ok = expect(checkIn.size == 0x20 && checkOut.size == 0x20 && checkIn.packet.Attr == 0 && checkOut.packet.Attr == 0 && std::strcmp(checkIn.packet.Data, "string2 Aldo") == 0 && std::strcmp(checkOut.packet.Data, "string2 Aldo") == 0, "monstrosity checks use name text and short size") && ok;
    const auto absent = CharacterParams2PlanFor({}, 33, 44, MsgStd::CannotInvite);
    ok = expect(absent.size == 0x24 && absent.packet.UniqueNo == 0 && absent.packet.ActIndex == 0 && std::strcmp(absent.packet.Data, "Para0 33 Para1 44") == 0, "absent character falls back to parameters") && ok;
    const auto truncated = CharacterParams2PlanFor({ .present = true, .name = "abcdefghijklmnopqrs" }, 0, 0, MsgStd::Examine);
    ok = expect(std::strcmp(truncated.packet.Data, "string2 abcdefghijklmno") == 0 && truncated.packet.Data[23] == '\0', "name text uses snprintf NUL truncation") && ok;
    return ok;
}
