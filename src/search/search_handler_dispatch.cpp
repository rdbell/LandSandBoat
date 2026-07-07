#include "search_handler_dispatch.h"

#include "search_request_type.h"

auto IsSearchPacketFrameLengthValid(const uint16 receivedLength, const uint16 declaredLength) -> bool
{
    return receivedLength == declaredLength && receivedLength >= 28;
}

auto SearchHandlerDispatchForRequestType(const uint8 packetType) -> SearchHandlerDispatch
{
    switch (packetType)
    {
        case TCP_SEARCH:
        case TCP_SEARCH_ALL:
            return SearchHandlerDispatch::SearchRequest;
        case TCP_SEARCH_COMMENT:
            return SearchHandlerDispatch::SearchComment;
        case TCP_GROUP_LIST:
            return SearchHandlerDispatch::GroupList;
        case TCP_AH_REQUEST:
        case TCP_AH_REQUEST_MORE:
            return SearchHandlerDispatch::AuctionHouseRequest;
        case TCP_AH_HISTORY_SINGLE:
        case TCP_AH_HISTORY_STACK:
            return SearchHandlerDispatch::AuctionHouseHistory;
        default:
            return SearchHandlerDispatch::Unknown;
    }
}
