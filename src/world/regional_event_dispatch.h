#pragma once

#include "common/ipp_message.h"

#include <functional>
#include <utility>

namespace worldipc
{

template <typename Event, typename Handle>
void HandleRegionalEvent(const IPP& source, const Event& message, Handle&& handle)
{
    std::invoke(handle, message.type, IPPMessage{ source, message.payload });
}

} // namespace worldipc
