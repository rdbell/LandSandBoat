#pragma once

#include "common/ipp.h"
#include "common/types/maybe.h"

#include <cstdint>
#include <string>

namespace world::ipc
{

auto LookupCharacterEndpoint(uint32_t charId) -> Maybe<IPP>;
auto LookupCharacterNameEndpoint(const std::string& charName) -> Maybe<IPP>;

} // namespace world::ipc
