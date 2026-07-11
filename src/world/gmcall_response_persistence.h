#pragma once

#include <cstdint>
#include <string_view>

namespace world::gmcall
{

void PersistResponse(uint32_t callId, std::string_view response);

} // namespace world::gmcall
