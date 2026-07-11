#pragma once

#include "common/ipc_structs.h"
#include "common/ipp.h"

namespace mapipc
{

// HandleGMCallRequest mirrors IPCClient::handleMessage_GMCallRequest.
// Map servers accept GMCallRequest on the bus but intentionally perform no
// work; world owns request processing and help-desk persistence. Parameters
// are retained for signature parity with other mapipc handlers.
inline void HandleGMCallRequest(const IPP& /*source*/, const ipc::GMCallRequest& /*message*/)
{
}

} // namespace mapipc
