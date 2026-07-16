/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "packets/s2c/0x052_eventucoff.h"

namespace passwards
{

// UpdateStringFrom converts the fixed-width client String field without
// reading past it. Bytes after the first NUL are excluded and non-ASCII bytes
// are retained unchanged, matching asStringFromUntrustedSource's behavior.
auto UpdateStringFrom(const uint8_t (&rawString)[16]) -> std::string;

// RuntimeCallbacks contains the host operations selected by PASSWARDS.
// onEventUpdate runs before the two EVENTUCOFF responses.
struct RuntimeCallbacks
{
    std::function<void(const std::string&)>              onEventUpdate;
    std::function<void(GP_SERV_COMMAND_EVENTUCOFF_MODE)> sendEventUCOff;
};

// Run invokes OnEventUpdate then sends EventRecvPending and CancelInput, in
// that order. Packet/entity ownership remains with the caller.
void Run(const uint8_t (&rawString)[16], const RuntimeCallbacks& callbacks);

} // namespace passwards
