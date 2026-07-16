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
#include <string>

namespace inspectmessage
{

// BazaarMessageFrom converts INSPECT_MESSAGE's fixed-width client field into
// the bounded value persisted by the handler. It retains bytes before the
// first NUL and never consumes the three protocol-reserved tail bytes.
auto BazaarMessageFrom(const uint8_t (&rawMessage)[123]) -> std::string;

// RuntimeTransition is the host-independent effect of attempting to persist
// an INSPECT_MESSAGE bazaar message. The in-memory message changes only when
// the prepared database update succeeds.
struct RuntimeTransition
{
    std::string preparedMessage;
    std::string bazaarMessage;
    bool        bazaarMessageUpdated = false;
};

auto TransitionFor(const uint8_t (&rawMessage)[123], const std::string& currentBazaarMessage, bool preparedUpdateSucceeded) -> RuntimeTransition;

} // namespace inspectmessage
