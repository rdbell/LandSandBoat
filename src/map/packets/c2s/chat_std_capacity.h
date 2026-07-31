#pragma once

#include <algorithm>
#include <cstddef>

namespace chatstdhelpers
{

constexpr std::size_t MaxMessageLength = 128;
constexpr std::size_t MessageOffset    = 6;

// BoundedMessageLength mirrors CHAT_STD's std::min<size_t> expression after
// the four-byte header size has been expanded. The unsigned subtraction is
// intentional: malformed short host values wrap before the 128-byte cap.
constexpr auto BoundedMessageLength(const std::size_t reportedPacketSize) -> std::size_t
{
    return std::min(reportedPacketSize - MessageOffset, MaxMessageLength);
}

} // namespace chatstdhelpers
