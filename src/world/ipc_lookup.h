#pragma once

#include "common/ipp.h"
#include "common/types/maybe.h"
#include "kill_session_zones.h"

#include <cstdint>
#include <string>
#include <vector>

namespace world::ipc
{

auto LookupCharacterEndpoint(uint32_t charId) -> Maybe<IPP>;
auto LookupCharacterNameEndpoint(const std::string& charName) -> Maybe<IPP>;
auto LookupPartyEndpoints(uint32_t partyId) -> std::vector<IPP>;
auto LookupAllianceEndpoints(uint32_t allianceId) -> std::vector<IPP>;
auto LookupLinkshellEndpoints(uint32_t linkshellId) -> std::vector<IPP>;
auto LookupUnityEndpoints(uint32_t unityId) -> std::vector<IPP>;
auto LookupKillSessionZones(uint32_t victimId) -> Maybe<worldipc::KillSessionZones>;

} // namespace world::ipc
