#pragma once

#include "common/ipp.h"
#include "common/types/maybe.h"

#include <cstdint>

namespace world::ipc
{

auto LookupCharacterEndpoint(uint32_t charId) -> Maybe<IPP>;

} // namespace world::ipc
