#include "test_char_inventory_linkshell_attachments_6898.h"

#include "map/char_inventory_linkshell_attachments.h"

#include <iostream>

auto runCharInventoryLinkshellAttachments6898SelfTests() -> bool
{
    constexpr auto expected = std::array<inventorylinkshellhelpers::Attachment, 2>{
        inventorylinkshellhelpers::Attachment{ SLOT_LINK1, 1 },
        inventorylinkshellhelpers::Attachment{ SLOT_LINK2, 2 },
    };
    const bool ok = inventorylinkshellhelpers::BuildAttachmentPlan() == expected;
    if (!ok)
    {
        std::cerr << "inventory linkshell attachments 6898 self-test failed\n";
    }
    return ok;
}
