#pragma once

#include <common/cbasetypes.h>

namespace loginHelpers
{

// CanAddCharacterCreationSlot mirrors the DATA A1 creation-slot loop. Slots
// are appended only while the visible character count is below content_ids.
inline auto CanAddCharacterCreationSlot(const uint32 visibleCharacterCount, const uint32 contentIDCount) -> bool
{
    return visibleCharacterCount < contentIDCount;
}

} // namespace loginHelpers
