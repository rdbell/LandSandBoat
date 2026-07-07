#pragma once

#include "common/cbasetypes.h"

enum class SearchHandlerDispatch : uint8
{
    Unknown             = 0,
    SearchRequest       = 1,
    SearchComment       = 2,
    GroupList           = 3,
    AuctionHouseRequest = 4,
    AuctionHouseHistory = 5,
};

auto IsSearchPacketFrameLengthValid(uint16 receivedLength, uint16 declaredLength) -> bool;
auto SearchHandlerDispatchForRequestType(uint8 packetType) -> SearchHandlerDispatch;
