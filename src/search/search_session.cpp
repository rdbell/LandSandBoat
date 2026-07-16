/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "search_session.h"

#include "common/logging.h"
#include "common/types/maybe.h"
#include "search.h"

#include <array>

namespace
{

constexpr std::size_t minSearchPacketSize = 28;

auto readExact(SearchSessionTransport& transport, uint8* data, const std::size_t length) -> bool
{
    std::size_t offset = 0;
    while (offset < length)
    {
        const auto received = transport.read(data + offset, length - offset);
        if (received == 0 || received > length - offset)
        {
            return false;
        }
        offset += received;
    }
    return true;
}

void finish(SearchSessionTransport& transport, const SearchSessionCleanup& cleanup)
{
    transport.close();
    if (cleanup)
    {
        cleanup();
    }
}

} // namespace

auto ServeSearchSession(SearchSessionTransport& transport, const SearchSessionProcessor& processor, const SearchSessionCleanup& cleanup) -> SearchSessionResult
{
    std::array<uint8, 2> header{};

    while (true)
    {
        const auto headerBytes = transport.read(header.data(), header.size());
        if (headerBytes == 0)
        {
            finish(transport, cleanup);
            return SearchSessionResult::EndOfFile;
        }
        if (headerBytes > header.size() || !readExact(transport, header.data() + headerBytes, header.size() - headerBytes))
        {
            finish(transport, cleanup);
            return SearchSessionResult::TruncatedFrame;
        }

        const auto length = static_cast<std::size_t>(header[0]) | (static_cast<std::size_t>(header[1]) << 8U);
        if (length < minSearchPacketSize || length > searchPacket::max_size)
        {
            finish(transport, cleanup);
            return SearchSessionResult::InvalidFrameLength;
        }

        auto frame = std::vector<uint8>(length);
        frame[0]   = header[0];
        frame[1]   = header[1];
        if (!readExact(transport, frame.data() + header.size(), frame.size() - header.size()))
        {
            finish(transport, cleanup);
            return SearchSessionResult::TruncatedFrame;
        }

        for (const auto& response : processor(frame))
        {
            if (response.size() < minSearchPacketSize || response.size() > searchPacket::max_size ||
                response[0] != static_cast<uint8>(response.size() & 0xFFU) || response[1] != static_cast<uint8>(response.size() >> 8U) ||
                !transport.write(response.data(), response.size()))
            {
                finish(transport, cleanup);
                return SearchSessionResult::WriteFailure;
            }
        }
    }
}
