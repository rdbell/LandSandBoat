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

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

// SearchSessionTransport is the synchronous, transport-neutral framing seam
// shared by Search listener tests. read() returns zero at EOF; write() returns
// false on failure. Implementations may deliberately return short reads.
class SearchSessionTransport
{
public:
    virtual ~SearchSessionTransport() = default;

    virtual auto read(uint8* data, std::size_t length) -> std::size_t = 0;
    virtual auto write(const uint8* data, std::size_t length) -> bool   = 0;
    virtual void close()                                                = 0;
};

enum class SearchSessionResult : uint8
{
    EndOfFile,
    TruncatedFrame,
    InvalidFrameLength,
    WriteFailure,
};

using SearchSessionProcessor = std::function<std::vector<std::vector<uint8>>(const std::vector<uint8>&)>;
using SearchSessionCleanup   = std::function<void()>;

// ServeSearchSession owns one bounded framed connection. It reassembles TCP
// reads into complete Search frames, invokes processor once per request, writes
// all responses in processor order, and always closes then runs cleanup before
// returning. Crypto, hash validation, dispatch, and data access intentionally
// remain with SearchHandler.
auto ServeSearchSession(SearchSessionTransport& transport, const SearchSessionProcessor& processor, const SearchSessionCleanup& cleanup = {}) -> SearchSessionResult;
