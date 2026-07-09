/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "version_lock.h"

#include <algorithm>
#include <utility>

namespace login::version_lock
{

auto normalizeClientVersion(const std::span<const std::uint8_t> clientBytes) -> std::string
{
    const auto prefix = clientBytes.first(std::min(clientBytes.size(), PrefixLength));
    const auto nul    = std::find(prefix.begin(), prefix.end(), std::uint8_t{ 0 });

    std::string normalized;
    normalized.reserve(static_cast<std::size_t>(nul - prefix.begin()) + std::char_traits<char>::length(MaskedSuffix));
    for (auto byte = prefix.begin(); byte != nul; ++byte)
    {
        normalized.push_back(static_cast<char>(*byte));
    }
    normalized += MaskedSuffix;
    return normalized;
}

auto normalizeExpectedVersion(const std::string_view expectedVersion) -> std::string
{
    auto normalized = std::string(expectedVersion.substr(0, PrefixLength));
    normalized += MaskedSuffix;
    return normalized;
}

auto compare(const std::span<const std::uint8_t> clientBytes, const std::string_view expectedVersion) -> Decision
{
    Decision decision{
        .clientVersion   = normalizeClientVersion(clientBytes),
        .expectedVersion = normalizeExpectedVersion(expectedVersion),
    };

    decision.mismatch = decision.expectedVersion != decision.clientVersion;
    if (!decision.mismatch)
    {
        return decision;
    }

    decision.direction = decision.expectedVersion < decision.clientVersion ? MismatchDirection::ServerTooOld : MismatchDirection::ClientTooOld;
    return decision;
}

auto applyLock(Decision decision, const std::uint8_t lockMode) -> Decision
{
    decision.fatal = false;
    if (!decision.mismatch)
    {
        return decision;
    }

    switch (lockMode)
    {
        case Strict:
            decision.fatal = true;
            break;
        case AllowNewer:
            decision.fatal = decision.expectedVersion > decision.clientVersion;
            break;
        default:
            break;
    }

    return decision;
}

auto evaluate(const std::span<const std::uint8_t> clientBytes,
              const std::string_view              expectedVersion,
              const std::uint8_t                  lockMode) -> Decision
{
    return applyLock(compare(clientBytes, expectedVersion), lockMode);
}

auto evaluateFlow(const std::span<const std::uint8_t> clientBytes,
                  const std::string_view              expectedVersion,
                  const std::function<std::uint8_t()>& readLockMode) -> FlowDecision
{
    FlowDecision flow{ .version = compare(clientBytes, expectedVersion) };
    if (!flow.version.mismatch)
    {
        return flow;
    }

    flow.lockMode = readLockMode();
    flow.version  = applyLock(std::move(flow.version), flow.lockMode);
    if (flow.version.fatal)
    {
        flow.responseLength = ResponseLength::VersionError;
    }
    return flow;
}

} // namespace login::version_lock
