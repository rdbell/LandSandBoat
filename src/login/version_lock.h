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

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <string_view>

namespace login::version_lock
{

inline constexpr std::size_t PrefixLength = 6;
inline constexpr auto        MaskedSuffix = "xx_x";
inline constexpr std::uint8_t Disabled     = 0;
inline constexpr std::uint8_t Strict       = 1;
inline constexpr std::uint8_t AllowNewer   = 2;

enum class MismatchDirection : std::uint8_t
{
    None,
    ServerTooOld,
    ClientTooOld,
};

struct Decision
{
    std::string       clientVersion;
    std::string       expectedVersion;
    bool              mismatch = false;
    bool              fatal    = false;
    MismatchDirection direction = MismatchDirection::None;
};

enum class ResponseLength : std::size_t
{
    VersionError = 0x24,
    KeyPacket    = 0x28,
};

struct FlowDecision
{
    Decision       version;
    std::uint8_t   lockMode      = Disabled;
    ResponseLength responseLength = ResponseLength::KeyPacket;
};

auto normalizeClientVersion(std::span<const std::uint8_t> clientBytes) -> std::string;
auto normalizeExpectedVersion(std::string_view expectedVersion) -> std::string;
auto compare(std::span<const std::uint8_t> clientBytes, std::string_view expectedVersion) -> Decision;
auto applyLock(Decision decision, std::uint8_t lockMode) -> Decision;
auto evaluate(std::span<const std::uint8_t> clientBytes, std::string_view expectedVersion, std::uint8_t lockMode) -> Decision;
auto evaluateFlow(std::span<const std::uint8_t> clientBytes,
                  std::string_view              expectedVersion,
                  const std::function<std::uint8_t()>& readLockMode) -> FlowDecision;

} // namespace login::version_lock
