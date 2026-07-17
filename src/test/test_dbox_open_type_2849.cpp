#include "test_dbox_open_type_2849.h"

#include "map/utils/dbox_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dbox open type 2849 self-test failed: " << label << '\n';
    }
    return condition;
}

// Production UCONTAINERTYPE pins (universal_container.h) without pulling
// universal_container headers into the pure capacity surface.
//   UCONTAINER_EMPTY = 0
//   UCONTAINER_TRADE = 4
//   UCONTAINER_AUCTION = 6
//   UCONTAINER_SEND_DELIVERYBOX = 7
//   UCONTAINER_RECV_DELIVERYBOX = 8
constexpr uint8 TypeEmpty            = 0;
constexpr uint8 TypeTrade            = 4;
constexpr uint8 TypeAuction          = 6;
constexpr uint8 TypeSendDeliveryBox  = 7;
constexpr uint8 TypeRecvDeliveryBox  = 8;

} // namespace

// Pure dual-wire expansion for dboxutilshelpers::{IsSendBoxOpen,IsRecvBoxOpen,
// IsAnyDeliveryBoxOpen} (slice 2849).
auto runDboxOpenType2849SelfTests() -> bool
{
    using dboxutilshelpers::IsAnyDeliveryBoxOpen;
    using dboxutilshelpers::IsRecvBoxOpen;
    using dboxutilshelpers::IsSendBoxOpen;

    bool ok = true;

    // IsSendBoxOpen: type == sendDeliveryBoxType
    ok = expect(IsSendBoxOpen(TypeSendDeliveryBox, TypeSendDeliveryBox), "send match") && ok;
    ok = expect(!IsSendBoxOpen(TypeRecvDeliveryBox, TypeSendDeliveryBox), "send vs recv") && ok;
    ok = expect(!IsSendBoxOpen(TypeEmpty, TypeSendDeliveryBox), "send vs empty") && ok;
    ok = expect(!IsSendBoxOpen(TypeTrade, TypeSendDeliveryBox), "send vs trade") && ok;
    ok = expect(!IsSendBoxOpen(TypeAuction, TypeSendDeliveryBox), "send vs auction") && ok;

    // IsRecvBoxOpen: type == recvDeliveryBoxType
    ok = expect(IsRecvBoxOpen(TypeRecvDeliveryBox, TypeRecvDeliveryBox), "recv match") && ok;
    ok = expect(!IsRecvBoxOpen(TypeSendDeliveryBox, TypeRecvDeliveryBox), "recv vs send") && ok;
    ok = expect(!IsRecvBoxOpen(TypeEmpty, TypeRecvDeliveryBox), "recv vs empty") && ok;
    ok = expect(!IsRecvBoxOpen(TypeTrade, TypeRecvDeliveryBox), "recv vs trade") && ok;
    ok = expect(!IsRecvBoxOpen(TypeAuction, TypeRecvDeliveryBox), "recv vs auction") && ok;

    // IsAnyDeliveryBoxOpen: send || recv
    ok = expect(IsAnyDeliveryBoxOpen(TypeSendDeliveryBox, TypeSendDeliveryBox, TypeRecvDeliveryBox), "any send") && ok;
    ok = expect(IsAnyDeliveryBoxOpen(TypeRecvDeliveryBox, TypeSendDeliveryBox, TypeRecvDeliveryBox), "any recv") && ok;
    ok = expect(!IsAnyDeliveryBoxOpen(TypeEmpty, TypeSendDeliveryBox, TypeRecvDeliveryBox), "any empty") && ok;
    ok = expect(!IsAnyDeliveryBoxOpen(TypeTrade, TypeSendDeliveryBox, TypeRecvDeliveryBox), "any trade") && ok;
    ok = expect(!IsAnyDeliveryBoxOpen(TypeAuction, TypeSendDeliveryBox, TypeRecvDeliveryBox), "any auction") && ok;

    // Dual-wire composition table: pure helpers match inline formula.
    const struct
    {
        uint8       type;
        bool        wantSend;
        bool        wantRecv;
        bool        wantAny;
        const char* label;
    } cases[] = {
        { TypeEmpty, false, false, false, "table empty" },
        { TypeTrade, false, false, false, "table trade" },
        { TypeAuction, false, false, false, "table auction" },
        { TypeSendDeliveryBox, true, false, true, "table send" },
        { TypeRecvDeliveryBox, false, true, true, "table recv" },
        { 1, false, false, false, "table syntes" },
        { 5, false, false, false, "table useitem" },
        { 9, false, false, false, "table past enum" },
        { 255, false, false, false, "table max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool gotSend = IsSendBoxOpen(c.type, TypeSendDeliveryBox);
        const bool gotRecv = IsRecvBoxOpen(c.type, TypeRecvDeliveryBox);
        const bool gotAny  = IsAnyDeliveryBoxOpen(c.type, TypeSendDeliveryBox, TypeRecvDeliveryBox);

        const bool inlineSend = c.type == TypeSendDeliveryBox;
        const bool inlineRecv = c.type == TypeRecvDeliveryBox;
        const bool inlineAny  = inlineSend || inlineRecv;

        ok = expect(gotSend == c.wantSend, c.label) && ok;
        ok = expect(gotRecv == c.wantRecv, c.label) && ok;
        ok = expect(gotAny == c.wantAny, c.label) && ok;
        ok = expect(gotSend == inlineSend, "dual-wire send matches inline") && ok;
        ok = expect(gotRecv == inlineRecv, "dual-wire recv matches inline") && ok;
        ok = expect(gotAny == inlineAny, "dual-wire any matches inline") && ok;
        ok = expect(gotAny == (gotSend || gotRecv), "any composes send||recv") && ok;
    }

    // Injected type constants: comparison uses host-supplied pins, not baked values.
    ok = expect(IsSendBoxOpen(99, 99), "injected send match") && ok;
    ok = expect(!IsSendBoxOpen(99, TypeSendDeliveryBox), "injected send mismatch") && ok;
    ok = expect(IsRecvBoxOpen(42, 42), "injected recv match") && ok;
    ok = expect(!IsRecvBoxOpen(42, TypeRecvDeliveryBox), "injected recv mismatch") && ok;
    ok = expect(IsAnyDeliveryBoxOpen(99, 99, 42), "injected any send path") && ok;
    ok = expect(IsAnyDeliveryBoxOpen(42, 99, 42), "injected any recv path") && ok;
    ok = expect(!IsAnyDeliveryBoxOpen(0, 99, 42), "injected any neither") && ok;

    // Production dual-wire path: GetType inject + UCONTAINER_SEND/RECV pins.
    ok = expect(IsSendBoxOpen(7, 7) && !IsRecvBoxOpen(7, 8), "production send path") && ok;
    ok = expect(IsRecvBoxOpen(8, 8) && !IsSendBoxOpen(8, 7), "production recv path") && ok;
    ok = expect(IsAnyDeliveryBoxOpen(7, 7, 8) && IsAnyDeliveryBoxOpen(8, 7, 8), "production any path") && ok;

    return ok;
}
