#pragma once

#include "entities/battle_entity.h"

#include <array>

namespace inventorylinkshellhelpers
{

struct Attachment
{
    SLOTTYPE slot;
    uint8    number;

    auto operator==(const Attachment&) const -> bool = default;
};

constexpr auto BuildAttachmentPlan() -> std::array<Attachment, 2>
{
    return { Attachment{ SLOT_LINK1, 1 }, Attachment{ SLOT_LINK2, 2 } };
}

} // namespace inventorylinkshellhelpers
