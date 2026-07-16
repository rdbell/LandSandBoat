/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "passwards_runtime.h"

#include "common/logging.h"

auto passwards::UpdateStringFrom(const uint8_t (&rawString)[16]) -> std::string
{
    return asStringFromUntrustedSource(rawString, sizeof(rawString));
}

void passwards::Run(const uint8_t (&rawString)[16], const RuntimeCallbacks& callbacks)
{
    callbacks.onEventUpdate(UpdateStringFrom(rawString));
    callbacks.sendEventUCOff(GP_SERV_COMMAND_EVENTUCOFF_MODE::EventRecvPending);
    callbacks.sendEventUCOff(GP_SERV_COMMAND_EVENTUCOFF_MODE::CancelInput);
}
