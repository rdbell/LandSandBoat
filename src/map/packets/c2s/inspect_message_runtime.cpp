/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "inspect_message_runtime.h"

#include "common/logging.h"

auto inspectmessage::BazaarMessageFrom(const uint8_t (&rawMessage)[123]) -> std::string
{
    return asStringFromUntrustedSource(rawMessage, sizeof(rawMessage) - 3);
}

auto inspectmessage::TransitionFor(const uint8_t (&rawMessage)[123], const std::string& currentBazaarMessage, const bool preparedUpdateSucceeded) -> RuntimeTransition
{
    auto transition = RuntimeTransition{
        .preparedMessage = BazaarMessageFrom(rawMessage),
        .bazaarMessage    = currentBazaarMessage,
    };

    if (preparedUpdateSucceeded)
    {
        transition.bazaarMessage        = transition.preparedMessage;
        transition.bazaarMessageUpdated = true;
    }

    return transition;
}
