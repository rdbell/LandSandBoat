#include "test_linkshell_process_item_3099.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldProcessLinkshellItem 3099 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddOnlineMember / DelOnlineMember item process gate for dual-wire
// cross-check (slice 3099):
//   itemNonNull && isLinkshellType
auto inlineShouldProcessLinkshellItem(const bool itemNonNull, const bool isLinkshellType) -> bool
{
    return itemNonNull && isLinkshellType;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldProcessLinkshellItem
// (itemNonNull && isLinkshellType; slice 3099). Dense 2² over
// itemNonNull × isLinkshellType. Residual 1355 pins still hold.
auto runLinkshellProcessItem3099SelfTests() -> bool
{
    using linkshellhelpers::FormatOnlineMemberNullWarning;
    using linkshellhelpers::OnlineMemberAlwaysReturnsFalse;
    using linkshellhelpers::ShouldAddMemberAfterOnlineLookup;
    using linkshellhelpers::ShouldEraseLinkshellAfterDelOnline;
    using linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd;
    using linkshellhelpers::ShouldProcessLinkshellItem;
    using linkshellhelpers::ShouldRejectNullOnlineMember;

    bool ok = true;

    // Residual 1355 pins still hold under dual-wire.
    ok = expect(ShouldProcessLinkshellItem(true, true), "residual: non-null ITEM_LINKSHELL processes") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true), "residual: null item rejects") && ok;
    ok = expect(!ShouldProcessLinkshellItem(true, false), "residual: non-linkshell type rejects") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, false), "residual: null non-type rejects") && ok;

    const struct
    {
        bool        itemNonNull;
        bool        isLinkshellType;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole.
        { true, true, true, "non-null ITEM_LINKSHELL processes" },

        // Dense 2² reject poles.
        { true, false, false, "non-null non-linkshell rejects" },
        { false, true, false, "null item rejects (type flag ignored)" },
        { false, false, false, "null non-type rejects" },

        // Residual polarity repeats for dual-wire stability.
        { true, true, true, "identity admit" },
        { false, true, false, "identity null reject" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldProcessLinkshellItem(c.itemNonNull, c.isLinkshellType);
        const bool inlineF = inlineShouldProcessLinkshellItem(c.itemNonNull, c.isLinkshellType);
        const bool wantPin = c.itemNonNull && c.isLinkshellType;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldProcessLinkshellItem dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldProcessLinkshellItem == pin formula") && ok;
    }

    // Pin composition: only both-true admits.
    ok = expect(ShouldProcessLinkshellItem(true, true), "both true must process") && ok;
    ok = expect(!ShouldProcessLinkshellItem(true, false), "type false must reject") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true), "item null must reject") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, false), "both false must reject") && ok;

    // Dense compose: full 2² boolean space free == inline == pin.
    for (const bool itemNonNull : { false, true })
    {
        for (const bool isLinkshellType : { false, true })
        {
            const bool got  = ShouldProcessLinkshellItem(itemNonNull, isLinkshellType);
            const bool want = itemNonNull && isLinkshellType;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldProcessLinkshellItem(itemNonNull, isLinkshellType),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Host path: AddOnlineMember / DelOnlineMember inject item flags.
    const struct
    {
        bool        itemNonNull;
        bool        isLinkshellType;
        bool        wantProcess;
        const char* label;
    } hosts[] = {
        { true, true, true, "non-null ITEM_LINKSHELL → process path" },
        { true, false, false, "non-null non-linkshell → skip item work" },
        { false, true, false, "null item → skip (type flag ignored)" },
        { false, false, false, "null non-type → skip item work" },
    };
    for (const auto& h : hosts)
    {
        const bool process = ShouldProcessLinkshellItem(h.itemNonNull, h.isLinkshellType);
        ok                 = expect(process == h.wantProcess, h.label) && ok;
        ok                 = expect(process == inlineShouldProcessLinkshellItem(h.itemNonNull, h.isLinkshellType),
                    "host compose free == inline") &&
             ok;
        ok = expect(process == (h.itemNonNull && h.isLinkshellType),
                    "host compose free == itemNonNull && isLinkshellType") &&
             ok;
        if (process)
        {
            ok = expect(ShouldAddMemberAfterOnlineLookup(true),
                        "compose: add-after-lookup residual on process path") &&
                 ok;
            ok = expect(!OnlineMemberAlwaysReturnsFalse(),
                        "compose: Add/DelOnlineMember still always returns false") &&
                 ok;
        }
    }

    // Production AddOnlineMember / DelOnlineMember path semantics.
    ok = expect(ShouldProcessLinkshellItem(true, true), "AddOnlineMember valid item → process path") && ok;
    ok = expect(!ShouldProcessLinkshellItem(false, false), "AddOnlineMember null item → skip path") && ok;
    ok = expect(ShouldProcessLinkshellItem(true, true), "DelOnlineMember valid item → process path") && ok;
    ok = expect(!ShouldProcessLinkshellItem(true, false), "DelOnlineMember non-linkshell → skip path") && ok;

    // Null short-circuit before process: host evaluates 3079 first.
    ok = expect(!ShouldRejectNullOnlineMember(false), "non-null char reaches process item gate") && ok;
    ok = expect(ShouldRejectNullOnlineMember(true), "null char rejects before process item") && ok;

    // Explicit dual-wire poles: free == itemNonNull && isLinkshellType for dense 2².
    for (const bool itemNonNull : { false, true })
    {
        for (const bool isLinkshellType : { false, true })
        {
            const bool got  = ShouldProcessLinkshellItem(itemNonNull, isLinkshellType);
            const bool want = itemNonNull && isLinkshellType;
            ok              = expect(got == want, "host inject dual-wire identity") && ok;
            ok              = expect(got == inlineShouldProcessLinkshellItem(itemNonNull, isLinkshellType),
                        "host inject free == inline") &&
                 ok;
        }
    }

    // Residual sibling gates remain orthogonal to this dual-wire surface.
    ok = expect(ShouldRejectNullOnlineMember(true), "sibling residual: null char still rejects") && ok;
    ok = expect(!ShouldRejectNullOnlineMember(false), "sibling residual: non-null char proceeds") && ok;
    ok = expect(ShouldLoadLinkshellOnOnlineAdd(false), "sibling residual: load on miss") && ok;
    ok = expect(!ShouldLoadLinkshellOnOnlineAdd(true), "sibling residual: no load on hit") && ok;
    ok = expect(ShouldAddMemberAfterOnlineLookup(true), "sibling residual: add when loaded") && ok;
    ok = expect(!ShouldAddMemberAfterOnlineLookup(false), "sibling residual: no add when null") && ok;
    ok = expect(!OnlineMemberAlwaysReturnsFalse(), "sibling residual: always returns false") && ok;
    ok = expect(FormatOnlineMemberNullWarning() == "PChar is null.", "sibling residual: null warn") && ok;
    ok = expect(ShouldEraseLinkshellAfterDelOnline(false), "sibling residual: erase empty") && ok;
    ok = expect(!ShouldEraseLinkshellAfterDelOnline(true), "sibling residual: keep non-empty") && ok;
    ok = expect(ShouldProcessLinkshellItem(true, true),
                "valid item must process even if later load/add/erase fails") &&
         ok;
    ok = expect(!ShouldProcessLinkshellItem(false, true),
                "null item must reject even if null/load gates would admit") &&
         ok;

    return ok;
}
