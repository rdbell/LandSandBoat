/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#include "common/cbasetypes.h"
#include "common/logging.h"

#include <asio/ssl.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

constexpr std::size_t HandlerSessionBufferSize              = 4096;
constexpr int         HandlerSessionKeepaliveIdleSeconds    = 5 * 60;
constexpr int         HandlerSessionKeepaliveIntervalSeconds = 5 * 60;
constexpr int         HandlerSessionKeepaliveProbeCount     = 10;

enum class handler_session_start_action : uint8
{
    NOOP,
    CONFIGURE_KEEPALIVE_AND_READ,
};

enum class handler_session_read_completion_action : uint8
{
    DISPATCH_READ,
    HANDLE_ERROR,
};

enum class handler_session_write_completion_action : uint8
{
    DISPATCH_WRITE,
    REPORT_ERROR,
};

constexpr auto handlerSessionStartAction(bool socketOpen) -> handler_session_start_action
{
    return socketOpen ? handler_session_start_action::CONFIGURE_KEEPALIVE_AND_READ : handler_session_start_action::NOOP;
}

constexpr auto handlerSessionReadCompletionAction(bool hasError) -> handler_session_read_completion_action
{
    return hasError ? handler_session_read_completion_action::HANDLE_ERROR : handler_session_read_completion_action::DISPATCH_READ;
}

constexpr auto handlerSessionWriteCompletionAction(bool hasError) -> handler_session_write_completion_action
{
    return hasError ? handler_session_write_completion_action::REPORT_ERROR : handler_session_write_completion_action::DISPATCH_WRITE;
}

class handler_session
: public std::enable_shared_from_this<handler_session>
{
public:
    handler_session(asio::ssl::stream<asio::ip::tcp::socket> socket);

    virtual ~handler_session() = default;

    void start();

    void do_read();

    virtual void handle_error(std::error_code ec, std::shared_ptr<handler_session> self) = 0;

    void do_write(std::size_t length);

    virtual void read_func()  = 0;
    virtual void write_func() = 0;

    std::string ipAddress;   // Store IP address in class -- once the file handle is invalid this can no longer be obtained from socket_
    std::string sessionHash; // Store session hash here additionally to clean up sockets easier

    asio::ssl::stream<asio::ip::tcp::socket> socket_;

    std::array<uint8, HandlerSessionBufferSize> buffer_ = {};
};
